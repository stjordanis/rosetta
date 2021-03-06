
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

#ifndef ROSETTA_SERVER_POST_USERS_HANDLER_HPP
#define ROSETTA_SERVER_POST_USERS_HANDLER_HPP

#include <string>
#include "common/include/exceptional_executor.hpp"
#include "http_server/include/connection/handlers/post_handler_base.hpp"

namespace rosetta {
namespace http_server {

using std::string;
using namespace rosetta::common;

class request;
class connection;


/// POST user data handler.
class post_users_handler final : public post_handler_base
{
public:

  /// Creates a POST handler for user data.
  post_users_handler (class request * request);

  /// Handles the given request.
  virtual void handle (connection_ptr connection, std::function<void()> on_success) override;

private:

  /// Evaluates the request.
  void evaluate (connection_ptr connection);

  /// Takes care of actions submitted by root account(s).
  void root_action (connection_ptr connection, const string & action);

  /// Root is allowed to change password of other accounts.
  void root_change_password (connection_ptr connection);

  /// Some root account is trying to change the role of some user.
  void root_change_role (connection_ptr connection);

  /// Some root account is trying to create a new user.
  void root_create_user (connection_ptr connection);

  /// Some root account is trying to delete a user.
  void root_delete_user (connection_ptr connection);

  /// Takes care of actions submitted by non-root accounts.
  void non_root_action (connection_ptr connection, const string & action);

  /// Changes the password of the given user.
  void change_password (connection_ptr connection, const string & username, const string & password);
};


} // namespace http_server
} // namespace rosetta

#endif // ROSETTA_SERVER_POST_USERS_HANDLER_HPP
