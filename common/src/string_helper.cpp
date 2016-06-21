
/*
 * Rosetta web server, copyright(c) 2016, Thomas Hansen, phosphorusfive@gmail.com.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License, as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <istream>
#include <boost/algorithm/string.hpp>
#include "common/include/string_helper.hpp"
#include "common/include/rosetta_exception.hpp"

using std::string;
using std::vector;
using std::istream;
using std::getline;
using std::istreambuf_iterator;
using namespace boost;

namespace rosetta {
namespace common {


string string_helper::get_line (streambuf & buffer, int size)
{
  // Reading next line from stream, and putting into vector buffer, for efficiency.
  vector<char> vec;
  istream stream (&buffer);

  // Iterating stream until CR/LF has been seen, and returning the line to caller, transformed
  // from its Cloaked version.
  while (stream.good ()) {

    // Get next character from stream, and interpret it according to the Cloaking rules.
    unsigned char idx = stream.get ();
    if (idx < 11)
      idx = '\n'; // LF
    else if (idx < 21)
      idx = '\r'; // CR
    else if (idx < 33)
      idx = ' '; // SP
    else if (idx > (unsigned char)190)
      continue; // IGNORE, garbage filling bytes.
    else if (idx > 127)
      idx = idx >> 1; // Bit-shift one bit down
    // else; Plain US ASCII character, in the range of [32-128>

    // Appending possibly transformed character into vector.
    vec.push_back (idx);

    // Checking if we have seen an entire line.
    if (vec.size() >= 2 && vec [vec.size() - 2] == '\r' && vec [vec.size() - 1] == '\n')
      break; // We have passed the CRLF sequence.
  }

  // Sanity checking line.
  if (vec.size() < 2)
    throw rosetta_exception ("Oops, bad line of data given to string_helper::get_line!");

  // Returning result to caller, ignoring CR/LF when creating return value.
  return string (vec.begin (), vec.end () - 2);;
}


unsigned char from_hex (unsigned char ch) 
{
  if (ch <= '9' && ch >= '0')
    ch -= '0';
  else if (ch <= 'f' && ch >= 'a')
    ch -= 'a' - 10;
  else if (ch <= 'F' && ch >= 'A')
    ch -= 'A' - 10;
  else 
    ch = 0;
  return ch;
}


string string_helper::decode_uri (const string & uri)
{
  // Will hold the decoded return URI value.
  string return_value;
  
  // Iterating through entire string, looking for either '+' or '%', which is specially handled.
  for (size_t idx = 0; idx < uri.length (); ++idx) {
    
    // Checking if this character should have special handling.
    if (uri [idx] == '+') {

      // '+' equals space " ".
      return_value += ' ';

    } else if (uri [idx] == '%' && uri.size() > idx + 2) {

      // '%' notation of character, followed by two characters.
      // The first character is bit shifted 4 places, and OR'ed with the value of the second character.
      return_value += (from_hex (uri [idx + 1]) << 4) | from_hex (uri [idx + 2]);
      idx += 2;

    } else {

      // Normal plain character.
      return_value += uri [idx];
    }
  }

  // Returning decoded URI to caller.
  return return_value;
}


} // namespace common
} // namespace rosetta
