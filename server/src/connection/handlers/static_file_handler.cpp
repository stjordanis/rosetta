
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
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "common/include/date.hpp"
#include "server/include/server.hpp"
#include "server/include/connection/request.hpp"
#include "server/include/connection/connection.hpp"
#include "server/include/exceptions/request_exception.hpp"
#include "server/include/connection/handlers/static_file_handler.hpp"

namespace rosetta {
namespace server {

using std::string;
using boost::system::error_code;
using namespace rosetta::common;


static_file_handler::static_file_handler (connection * connection, request * request, const string & extension)
  : request_handler (connection, request),
    _extension (extension)
{ }


void static_file_handler::handle (exceptional_executor x, std::function<void (exceptional_executor x)> functor)
{
  // Retrieving URI from request, removing initial "/" from URI.
  string uri = _request->envelope().get_uri().substr (1);

  // Breaking up URI into components, and sanity checking each component, to verify client is not requesting an illegal URI.
  std::vector<string> entities;
  boost::split (entities, uri, boost::is_any_of ("/"));
  bool has_error = false;
  for (string & idx : entities) {

    if (idx == "")
      has_error = true;

    if (idx.find ("..") != string::npos) // Request is probably trying to access files outside of the main www-root folder.
      has_error = true;

    if (idx.find ("~") == 0) // Linux backup file or folder.
      has_error = true;

    if (idx.find (".") == 0) // Linux hidden file or folder, or a file without a name, and only extension.
      has_error = true;
  }

  // Checking if we have an error, and if so, writing status error 404, and returning early.
  if (has_error) {
    _request->write_error_response (x, 404);
    return;
  }

  // Retrieving root path.
  const static string WWW_ROOT_PATH = _connection->server()->configuration().get<string> ("www-root", "www-root/");
  
  // Figuring out entire physical path of file.
  string path = WWW_ROOT_PATH + uri;

  // Making sure file exists.
  if (!boost::filesystem::exists (path)) {

      // Writing error status response, and returning early.
      _request->write_error_response (x, 404);
      return;
  }

  // Checking if client passed in an "If-Modified-Since" header, and if so, handle it accordingly.
  string if_modified_since = _request->envelope().get_header("If-Modified-Since");
  if (if_modified_since != "") {

    // We have an "If-Modified-Since" HTTP header, checking if file was tampered with since that date.
    date if_modified_date = date::parse (if_modified_since);
    date file_modify_date = date::from_file_change (path);

    // Comparing dates.
    if (file_modify_date > if_modified_date) {

      // File has been tampered with since the "If-Modified-Since" HTTP header, returning file in response.
      write_file (path, x, functor);
    } else {

      // File has not been tampered with since the "If-Modified-Since" HTTP header, returning 304 response, without file content.
      write_status (304, x, [this, functor] (exceptional_executor x) {

        // Building our request headers.
        std::vector<std::tuple<string, string> > headers { {"Date", date::now ().to_string ()} };

        // Writing HTTP headers to connection.
        write_headers (headers, x, functor);
      });
    }
  } else {

    // Making sure we add the Last-Modified header for our file, to help clients and proxies cache the file.
    write_header ("Last-Modified", date::from_file_change (path).to_string (), x, [this, path, functor] (exceptional_executor x) {

      // Then writing actual file.
      write_file (path, x, functor);
    });
  }
}


} // namespace server
} // namespace rosetta
