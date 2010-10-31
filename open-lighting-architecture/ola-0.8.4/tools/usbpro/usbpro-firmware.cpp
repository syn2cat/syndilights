/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * usbpro-firmware.cpp
 * Copyright (C) 2005 Simon Newton
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <sysexits.h>
#include <termios.h>
#include <ola/Logging.h>
#include <ola/Closure.h>
#include <ola/network/SelectServer.h>

#include <iostream>
#include <fstream>
#include <string>

#include "plugins/usbpro/UsbWidget.h"
#include "tools/usbpro/usbpro-common.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using ola::plugin::usbpro::UsbWidget;
using ola::network::SelectServer;

static const char DEFAULT_DEVICE[] = "/dev/ttyUSB0";
static const char DEFAULT_FIRMWARE[] = "main.bin";
static const int PAUSE_DELAY = 1000;  // ms before starting upload
static const int ABORT_TIMEOUT = 10 * 1000;  // ms seconds before aborting

typedef struct {
  bool help;
  ola::log_level log_level;
  string firmware;
  string device;
} options;


class FirmwareTransferer: public ola::plugin::usbpro::WidgetListener {
  public:
    FirmwareTransferer(ifstream *file,
                       UsbWidget *widget,
                       ola::network::SelectServer *ss):
        m_sucessful(false),
        m_firmware(file),
        m_widget(widget),
        m_ss(ss) {
      widget->SetMessageHandler(this);
    }

    bool SendReprogram();

    void HandleMessage(UsbWidget *widget,
                       uint8_t label,
                       unsigned int length,
                       const uint8_t *data);
    bool SendNextChunk();
    void AbortTransfer() {
      m_ss->Terminate();
    }
    void StartTransfer() {
      SendNextChunk();
    }
    bool WasSucessfull() const { return m_sucessful; }

  private:
    enum { FLASH_STATUS_LENGTH = 4 };
    enum { FLASH_PAGE_LENGTH = 64 };

    bool m_sucessful;
    ifstream *m_firmware;
    UsbWidget *m_widget;
    SelectServer *m_ss;

    static const uint8_t REPROGRAM_LABEL = 1;
    static const uint8_t FLASH_PAGE_LABEL = 2;
    static const char REPLY_SUCCESS[];
};


const char FirmwareTransferer::REPLY_SUCCESS[] = "TRUE";


/*
 * Send the re-program request
 */
bool FirmwareTransferer::SendReprogram() {
  return m_widget->SendMessage(REPROGRAM_LABEL, 0, NULL);
}


/*
 * Handle the flash page replies
 */
void FirmwareTransferer::HandleMessage(UsbWidget *widget,
                                       uint8_t label,
                                       unsigned int length,
                                       const uint8_t *data) {
  if (widget != m_widget) {
    OLA_WARN << "Something went really wrong...";
    return;
  }

  if (label != FLASH_PAGE_LABEL || length != FLASH_STATUS_LENGTH)
    return;

  if (0 == memcmp(data, REPLY_SUCCESS, sizeof(FLASH_STATUS_LENGTH))) {
    if (!SendNextChunk() || m_sucessful)
      m_ss->Terminate();
  } else {
    OLA_FATAL << "Bad response from widget:" << string((const char*) data, 4);
    m_ss->Terminate();
  }
}


/*
 * Send the next chunk of the firmware file
 */
bool FirmwareTransferer::SendNextChunk() {
  uint8_t page[FLASH_PAGE_LENGTH];
  m_firmware->read(reinterpret_cast<char*>(page),
                   FLASH_PAGE_LENGTH);
  std::streamsize size = m_firmware->gcount();

  if (!size) {
    m_sucessful = true;
    cout << endl;
    return true;
  }
  cout << ".";
  fflush(stdout);
  return m_widget->SendMessage(FLASH_PAGE_LABEL, size, page);
}


/*
 * Parse our command line options
 */
void ParseOptions(int argc, char *argv[], options *opts) {
  static struct option long_options[] = {
      {"device", required_argument, 0, 'd'},
      {"firmware", required_argument, 0, 'f'},
      {"help", no_argument, 0, 'h'},
      {"log-level", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };

  int option_index = 0;

  while (1) {
    int c = getopt_long(argc, argv, "d:f:hl:", long_options, &option_index);

    if (c == -1)
      break;

    switch (c) {
      case 0:
        break;

      case 'd':
        opts->device = optarg;
        break;

      case 'f':
        opts->firmware = optarg;
        break;

      case 'h':
        opts->help = true;
        break;

      case 'l':
        switch (atoi(optarg)) {
          case 0:
            // nothing is written at this level
            // so this turns logging off
            opts->log_level = ola::OLA_LOG_NONE;
            break;
          case 1:
            opts->log_level = ola::OLA_LOG_FATAL;
            break;
          case 2:
            opts->log_level = ola::OLA_LOG_WARN;
            break;
          case 3:
            opts->log_level = ola::OLA_LOG_INFO;
            break;
          case 4:
            opts->log_level = ola::OLA_LOG_DEBUG;
            break;
          default :
            break;
        }
        break;
      case '?':
        break;
      default:
       break;
    }
  }
  return;
}


/*
 * Display the help message
 */
void DisplayHelpAndExit(char *argv[]) {
  cout << "Usage: " << argv[0] <<
  " -d <device> -f <firmware_file>\n"
  "\n"
  "Flash the firmware on an Enttec USB Pro device.\n"
  "\n"
  "  -d <device_path>   The path to the device.\n"
  "  -f <firmware_file> The path to the firmware to use.\n"
  "  -h, --help         Display this help message and exit.\n"
  "  -l, --log-level <level>  Set the loggging level 0 .. 4.\n"
  << endl;
  exit(0);
}


/*
 * Flashes the device
 */
int main(int argc, char *argv[]) {
  options opts;
  opts.log_level = ola::OLA_LOG_WARN;
  opts.help = false;
  opts.firmware = DEFAULT_FIRMWARE;
  opts.device = DEFAULT_DEVICE;
  ParseOptions(argc, argv, &opts);

  if (opts.help)
    DisplayHelpAndExit(argv);
  ola::InitLogging(opts.log_level, ola::OLA_LOG_STDERR);

  ifstream firmware_file(opts.firmware.data());

  if (!firmware_file.is_open()) {
    OLA_FATAL << "Can't open the firmware file " << opts.firmware << ": " <<
      strerror(errno);
    exit(1);
  }

  ola::network::SelectServer ss;

  int fd = ConnectToWidget(opts.device);
  if (fd < 0)
    exit(EX_UNAVAILABLE);

  UsbWidget widget(MySelectServerAdaptor(&ss), fd);

  FirmwareTransferer transferer(&firmware_file, &widget, &ss);

  if (!transferer.SendReprogram()) {
    OLA_FATAL << "Send message failed";
    exit(1);
  }

  ss.RegisterSingleTimeout(
      PAUSE_DELAY,
      ola::NewSingleClosure(&transferer, &FirmwareTransferer::StartTransfer));
  widget.SetOnRemove(
      ola::NewSingleClosure(&transferer, &FirmwareTransferer::AbortTransfer));
  ss.RegisterSingleTimeout(
      ABORT_TIMEOUT,
      ola::NewSingleClosure(&transferer, &FirmwareTransferer::AbortTransfer));
  ss.Run();

  firmware_file.close();
  return !transferer.WasSucessfull();
}
