/**
 * Master thesis
 * by Alf-Andre Walla 2016-2017
 *
**/
#include <os>
#include <net/inet4>
#include <cstdio>
#include "../LiveUpdate/liveupdate.hpp"
static void* LIVEUPD_LOCATION = (void*) 0x8000000; // at 128mb

//#define ORIGIN
using storage_func_t = liu::LiveUpdate::storage_func;

static void save_state(liu::Storage& storage, const liu::buffer_t*)
{
  storage.add_int(1, 1337);
  storage.put_marker(1);
}
static void restore_state(liu::Restore& thing)
{
  assert(thing.get_id() == 1);
  printf("Restored integer %d\n", thing.as_int());
  thing.pop_marker(1);
}

void Service::start()
{
#ifdef ORIGIN
  auto& inet = net::Inet4::ifconfig<0>(
        { 10,0,0,40 },     // IP
        { 255,255,255,0 }, // Netmask
        { 10,0,0,1 },      // Gateway
        { 10,0,0,1 });     // DNS
  printf("This is the origin service @ %s\n", inet.link_addr().to_string().c_str());
  printf("Connecting to new system...\n");
  inet.tcp().connect({{10,0,0,43}, 1337},
    [] (auto conn) {
      printf("Connected to new instance, saving state...\n");
      auto size = liu::LiveUpdate::store(LIVEUPD_LOCATION, save_state);
      printf("Sending data...\n");
      conn->write(LIVEUPD_LOCATION, size);
      OS::shutdown();
    });
#else
  auto& inet = net::Inet4::ifconfig<0>(
        { 10,0,0,43 },     // IP
        { 255,255,255,0 }, // Netmask
        { 10,0,0,1 },      // Gateway
        { 10,0,0,1 });     // DNS
  printf("This is the new service @ %s\n", inet.link_addr().to_string().c_str());
  printf("Waiting for state...\n");
  inet.tcp().listen(1337,
    [] (auto conn) {
      printf("Got connection from old instance\n");
      conn->on_read(32768,
        [] (auto buf, size_t len) {
          printf("Received %u bytes\n", (unsigned) len);
          liu::LiveUpdate::resume_from_heap(buf.get(), restore_state);
          printf("Success\n");
          OS::shutdown();
        });
    });
#endif
}
