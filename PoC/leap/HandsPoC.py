#!/usr/bin/python
################################################################################
# Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               #
# Leap Motion proprietary and confidential. Not for distribution.              #
# Use subject to the terms of the Leap Motion SDK Agreement available at       #
# https://developer.leapmotion.com/sdk_agreement, or another agreement         #
# between Leap Motion and you, your company or other organization.             #
################################################################################

import Leap, sys
import serial, time, os
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture

tty_USB = "/dev/tty.usbserial-A7005FGN"

ser = serial.Serial(port = tty_USB, baudrate=9600)
ser.close()
ser.open()
time.sleep(1)

class SampleListener(Leap.Listener):
    def on_init(self, controller):
        print "Initialized"

    def on_connect(self, controller):
        print "Connected"

        # Enable gestures
        controller.enable_gesture(Leap.Gesture.TYPE_CIRCLE);
        controller.enable_gesture(Leap.Gesture.TYPE_KEY_TAP);
        controller.enable_gesture(Leap.Gesture.TYPE_SCREEN_TAP);
        controller.enable_gesture(Leap.Gesture.TYPE_SWIPE);

    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        print "Disconnected"

    def on_exit(self, controller):
        print "Exited"

    def on_frame(self, controller):
        # Get the most recent frame and report some basic information
        frame = controller.frame()

        ##print "Frame id: %d, timestamp: %d, hands: %d, fingers: %d, tools: %d, gestures: %d" % (
        ##      frame.id, frame.timestamp, len(frame.hands), len(frame.fingers), len(frame.tools), len(frame.gestures()))

        if not frame.hands.empty:
            # Get the first hand
            hand = frame.hands[0]

            # Check if the hand has any fingers
            fingers = hand.fingers

            # Check if there is a fist
            if hand and fingers.empty:
                print "We have a fist, so let's make the ball vanish"
                if ser.isOpen():
                    ser.write(str(0))
                    time.sleep(0.3)
#            if len(frame.hands) == 1 and len(fingers) == 5:
#                print "We have a one full hand, so let's make the ball appear"
#                if ser.isOpen():
#                    ser.write(str(1))
#                    time.sleep(0.3)
            if len(frame.hands) == 2 and len(fingers) == 5:
                print "We have a two full hands, so let's make the balls appear"
                if ser.isOpen():
                    ser.write(str(2))
                    time.sleep(0.3)
            if len(frame.hands) == 2 and len(fingers) == 2:
                print "Bye Bye"
                ##if ser.isOpen():
                ##    ser.write(str(9))
                time.sleep(0.3)

            if not fingers.empty:
                # Calculate the hand's average finger tip position
                avg_pos = Leap.Vector()
                for finger in fingers:
                    avg_pos += finger.tip_position
                avg_pos /= len(fingers)
                ##print "Hand has %d fingers, average finger tip position: %s" % (
                ##      len(fingers), avg_pos)

            # Get the hand's sphere radius and palm position
            ##print "Hand sphere radius: %f mm, palm position: %s" % (
            ##      hand.sphere_radius, hand.palm_position[0])
            #print "Number of Hands: %s - Number of fingers: %s - Palm position: %s" % ( len(frame.hands), len(fingers), hand.palm_position[0])
            if len(frame.hands) == 1 and len(fingers) == 5 and -170 < hand.palm_position[0] < -85:
                print "w5"
                if ser.isOpen():
                    ser.write(str(4))
                    time.sleep(0.3)
            if len(frame.hands) == 1 and len(fingers) == 5 and -84 < hand.palm_position[0] < 0:
                print "w10"
                if ser.isOpen():
                    ser.write(str(5))
                    time.sleep(0.3)
            if len(frame.hands) == 1 and len(fingers) == 5 and 1 < hand.palm_position[0] < 85:
                print "w15"
                if ser.isOpen():
                    ser.write(str(6))
                    time.sleep(0.3)
            if len(frame.hands) == 1 and len(fingers) == 5 and 86 < hand.palm_position[0] < 170:
                print "w20"
                if ser.isOpen():
                    ser.write(str(7))
                    time.sleep(0.3)


            # Get the hand's normal vector and direction
            normal = hand.palm_normal
            direction = hand.direction

            # Calculate the hand's pitch, roll, and yaw angles
            ##print "Hand pitch: %f degrees, roll: %f degrees, yaw: %f degrees" % (
            ##    direction.pitch * Leap.RAD_TO_DEG,
            ##    normal.roll * Leap.RAD_TO_DEG,
            ##    direction.yaw * Leap.RAD_TO_DEG)

            # Gestures
            for gesture in frame.gestures():
                if gesture.type == Leap.Gesture.TYPE_CIRCLE:
                    circle = CircleGesture(gesture)

                    # Determine clock direction using the angle between the pointable and the circle normal
                    if circle.pointable.direction.angle_to(circle.normal) <= Leap.PI/4:
                        clockwiseness = "clockwise"
                    else:
                        clockwiseness = "counterclockwise"

                    # Calculate the angle swept since the last frame
                    swept_angle = 0
                    if circle.state != Leap.Gesture.STATE_START:
                        previous_update = CircleGesture(controller.frame(1).gesture(circle.id))
                        swept_angle =  (circle.progress - previous_update.progress) * 2 * Leap.PI

                    ##print "Circle id: %d, %s, progress: %f, radius: %f, angle: %f degrees, %s" % (
                    ##        gesture.id, self.state_string(gesture.state),
                    ##        circle.progress, circle.radius, swept_angle * Leap.RAD_TO_DEG, clockwiseness)

                if gesture.type == Leap.Gesture.TYPE_SWIPE:
                    swipe = SwipeGesture(gesture)
                    ##print "Swipe id: %d, state: %s, position: %s, direction: %s, speed: %f" % (
                    ##        gesture.id, self.state_string(gesture.state),
                    ##        swipe.position, swipe.direction, swipe.speed)

                if gesture.type == Leap.Gesture.TYPE_KEY_TAP:
                    keytap = KeyTapGesture(gesture)
                    print "Key Tap id: %d, %s, position: %s, direction: %s" % (
                            gesture.id, self.state_string(gesture.state),
                            keytap.position, keytap.direction )
                    if ser.isOpen():
                      ser.write(str(3))
                      time.sleep(0.3)

                if gesture.type == Leap.Gesture.TYPE_SCREEN_TAP:
                    screentap = ScreenTapGesture(gesture)
                    ##print "Screen Tap id: %d, %s, position: %s, direction: %s" % (
                    ##        gesture.id, self.state_string(gesture.state),
                    ##        screentap.position, screentap.direction )

        #if not (frame.hands.empty and frame.gestures().empty):
        #    print ""

    def state_string(self, state):
        if state == Leap.Gesture.STATE_START:
            return "STATE_START"

        if state == Leap.Gesture.STATE_UPDATE:
            return "STATE_UPDATE"

        if state == Leap.Gesture.STATE_STOP:
            return "STATE_STOP"

        if state == Leap.Gesture.STATE_INVALID:
            return "STATE_INVALID"

def main():
    # Create a sample listener and controller
    listener = SampleListener()
    controller = Leap.Controller()

    # Have the sample listener receive events from the controller
    controller.add_listener(listener)

    # Keep this process running until Enter is pressed
    print "Press Enter to quit..."
    sys.stdin.readline()

    # Remove the sample listener when done
    controller.remove_listener(listener)


if __name__ == "__main__":
    main()
