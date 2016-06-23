
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

#include <tuple>
#include <vector>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "common/include/date.hpp"
#include "server/include/server.hpp"
#include "server/include/connection/request.hpp"
#include "server/include/connection/connection.hpp"
#include "server/include/exceptions/request_exception.hpp"
#include "server/include/connection/handlers/error_handler.hpp"

namespace rosetta {
namespace server {

using std::string;
using boost::system::error_code;
using namespace rosetta::common;


error_handler::error_handler (connection * connection, request * request, int status_code)
  : request_handler (connection, request),
    _status_code (status_code)
{ }


void error_handler::handle (exceptional_executor x, std::function<void (exceptional_executor x)> functor)
{
  // Writing status code.
  write_status (_status_code, x, [this, x, functor] (exceptional_executor x) {

    // Figuring out which file to serve.
    string error_file = "error-pages/" + boost::lexical_cast<string> (_status_code) + ".html";
    write_file (error_file, x, [functor] (exceptional_executor x) {
      // Letting x go out of scope, without invoking functor, to close connection.
    });
  });
}


} // namespace server
} // namespace rosetta
