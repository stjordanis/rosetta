
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

#ifndef ROSETTA_SERVER_REQUEST_HANDLER_BASE_HPP
#define ROSETTA_SERVER_REQUEST_HANDLER_BASE_HPP

#include <tuple>
#include <vector>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

using std::string;
using namespace boost::filesystem;

namespace rosetta {
namespace http_server {

class request;
class connection;
typedef std::shared_ptr<connection> connection_ptr;

// Helpers for HTTP headers.
typedef std::tuple<string, string> collection_type;
typedef std::vector<collection_type> collection;


/// Common base class for all HTTP handlers.
class request_handler_base : public boost::noncopyable
{
public:

  /// Handles the given request.
  virtual void handle (connection_ptr connection, std::function<void()> on_success) = 0;

protected:

  /// Protected constructor.
  request_handler_base (class request * request);

  /// Writing given HTTP headetr with given value back to client.
  void write_status (connection_ptr connection, unsigned int status_code, std::function<void()> on_success);

  /// Writes a single HTTP header, with the given name/value combination back to client.
  void write_header (connection_ptr connection, const string & key, const string & value, std::function<void()> on_success);

  /// Writing given HTTP header collection back to client.
  void write_headers (connection_ptr connection, collection headers, std::function<void()> on_success);

  /// Writes back the standard HTTP headers back to client, that the server is configured to pass back on every response.
  void write_standard_headers (connection_ptr connection, std::function<void()> on_success);

  /// Ensures that the envelope of the response is flushed, and one empty line with CR/LF is written back to the client.
  void ensure_envelope_finished (connection_ptr connection, std::function<void()> on_success);

  /// Writes success return to client.
  void write_success_envelope (connection_ptr connection, std::function<void()> on_success);

  /// Returns request for this instance.
  request * request() { return _request; }

private:

  /// The request that owns this instance.
  class request * _request;
};


} // namespace http_server
} // namespace rosetta

#endif // ROSETTA_SERVER_REQUEST_HANDLER_BASE_HPP
