#ifndef ACCOUNT_HTTP_SERVER_H
#define ACCOUNT_HTTP_SERVER_H

#include <atomic>
#include <cstdint>
#include <future>
#include <player_metadata_t.hpp>

enum server_type_t : uint8_t { creating_account = 0, loading_account };

class account_http_server_t {
public:
  account_http_server_t(const server_type_t &server_type)
      : m_server_type{server_type}, m_account_loaded{} {}


    bool poll_authenticated() {
        return m_authenticated.load();
    }

    player_metadata_t get_authenticated_account() {

    }

private:
  std::atomic<bool> m_authenticated{false};
  server_type_t m_server_type;
  player_metadata_t m_account_loaded;
};

#endif