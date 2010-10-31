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
 * TokenBucket.cpp
 * Token Bucket implementation.
 * Copyright (C) 2010 Simon Newton
 */

#include <algorithm>
#include "olad/TokenBucket.h"

namespace ola {

/*
 * Check if we have enough tokens for an operation. Assuming there is enough
 * tokens, the count is decremented by one.
 * @param now the current time
 * @returns true if there is enough tokens, false otherwise
 */
bool TokenBucket::GetToken(const TimeStamp &now) {
  Count(now);
  if (m_count > 0) {
    m_count--;
    return true;
  }
  return false;
}


/*
 * Get the number of tokens in the bucket
 */
unsigned int TokenBucket::Count(const TimeStamp &now) {
  int64_t delta = (now - m_last).AsInt();
  unsigned int tokens = delta * m_rate / USEC_IN_SECONDS;

  m_count = std::min(m_max, m_count + tokens);
  if (tokens)
    m_last += ola::TimeInterval(tokens * USEC_IN_SECONDS / m_rate);
  return m_count;
}
}  // ola
