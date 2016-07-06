
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

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "http_server/include/server.hpp"
#include "http_server/include/connection/request.hpp"
#include "http_server/include/connection/connection.hpp"
#include "http_server/include/exceptions/request_exception.hpp"
#include "http_server/include/connection/create_request_handler.hpp"

namespace rosetta {
namespace http_server {
  
using std::string;
using boost::system::error_code;
using namespace rosetta::common;


request_ptr request::create ()
{
  return std::shared_ptr<request> (new request ());
}


request::request ()
  : _envelope (this)
{ }


void request::handle (connection_ptr connection)
{
  // Reading envelope.
  _envelope.read (connection, [this, connection] () {

    // Killing deadline timer while we handle request.
    connection->set_deadline_timer (-1);
    _request_handler = create_request_handler (connection, this);
    _request_handler->handle (connection, [this, connection] () {

      // Request is now finished handled, and we need to determine if we should keep connection alive or not.
      if (_envelope.header ("Connection") == "close") {

        // Closing connection
        connection->close();
      } else {

        // Keep-Alive Connection.
        connection->handle();
      }
    });
  });
}


void request::write_error_response (connection_ptr connection, int status_code)
{
  // Creating an error handler.
  _request_handler = create_request_handler (connection, this, status_code);
  _request_handler->handle (connection, [this, connection] () {

    // Closing connection on everything that are error requests.
    connection->close();
  });
}


} // namespace http_server
} // namespace rosetta
