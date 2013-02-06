/*
 * CCCHH Door Information System for UNIX
 * Author: hanez, Haeger
 * Thanks to: atari, beh and eisbaer
 * 
 * License (Beerware License):
 * As long as you retain this notice you can do whatever you want with this 
 * stuff. If we meet some day, and you think this stuff is worth it, you can buy 
 * me a beer in return hanez and all other contributers
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <json.h>
#if LIBNOTIFY
#include <libnotify/notify.h>
#endif

#include "pesthoernchen.h"

struct string {
  char *ptr;
  size_t len;
};

static char *name      = "Dooris for UNIX";
static char *statusurl = "http://www.hamburg.ccc.de/dooris/json.php";
static char *agent     = "Dooris-for-UNIX/0.1";

int user_count         = 0;
int old_user_count     = 0;
// Count of no real users like the router etc. Adjust this before compiling
int not_real_users     = 1;

// The delay for polling the dooris service. Adjust this before compiling
int delay              = 120000; // ms. aka 2 minutes

bool door_open         = false;
bool old_door_open     = false;

GtkStatusIcon *tray_icon;

bool do_it(bool);

void invoke_notification();

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

void tray_icon_on_click(GtkStatusIcon *status_icon, gpointer user_data) {
  do_it(false);
}

void get_bouncer_data() {
  CURL *curl;
  CURLcode res;
  
  struct json_object *response_json_object;
  struct json_object *door_json_object;
  struct json_object *dhcp_json_object;
  struct json_object *leaf_json_object;
  
  int door_status;
  int door_last_change;
  int door_last_update;

  int dhcp_status;
  int dhcp_last_change;
  int dhcp_last_update;

  struct string s;

  printf("Called get_bouncer_data()\n");

  old_user_count = user_count;
  old_door_open  = door_open;

  init_string(&s);
  
  curl = curl_easy_init();
  if(!curl) {
    fprintf(stderr, "curl_easy_init() failed.\n");
    return;
  }

  curl_easy_setopt(curl, CURLOPT_URL, statusurl);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);

  res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    return;
  }

  response_json_object = json_tokener_parse(s.ptr);
  free(s.ptr);

  // door objects
  door_json_object = json_object_object_get(response_json_object, "door");

  leaf_json_object = json_object_object_get(door_json_object, "status");
  door_status = json_object_get_int(leaf_json_object);
  
  leaf_json_object = json_object_object_get(door_json_object, "last_change");
  door_last_change = json_object_get_int(leaf_json_object);

  leaf_json_object = json_object_object_get(door_json_object, "last_update");
  door_last_update = json_object_get_int(leaf_json_object);
  // free door object
  json_object_put(door_json_object);

  // dhcp objects
  dhcp_json_object = json_object_object_get(response_json_object, "router");
  
  leaf_json_object = json_object_object_get(dhcp_json_object, "dhcp");
  dhcp_status = json_object_get_int(leaf_json_object);
  
  leaf_json_object = json_object_object_get(dhcp_json_object, "last_change");
  dhcp_last_change = json_object_get_int(leaf_json_object);

  leaf_json_object = json_object_object_get(dhcp_json_object, "last_update");
  dhcp_last_update = json_object_get_int(leaf_json_object);
  // free dhcp object
  json_object_put(dhcp_json_object);

  // free leaf object
  json_object_put(leaf_json_object);
  
  // free response object
  json_object_put(response_json_object);
  
  printf("Door status: = %d\n", door_status);
  printf("Door last change: = %d\n", door_last_change);
  printf("Door last update: = %d\n", door_last_update);

  printf("DHCP status: = %d\n", dhcp_status - not_real_users);
  printf("DHCP last change: = %d\n", dhcp_last_change);
  printf("DHCP last update: = %d\n", dhcp_last_update);

  if (door_status == 1) {
    door_open = false;
  } else if (door_status == 0) {
    door_open = true;
  }
  user_count = dhcp_status - not_real_users;
}

void invoke_notification() {
  char x[100];
  char door[100];
  
  if (door_open == true) {
    snprintf(door, 100, "Door open.");
  } else {
    snprintf(door, 100, "Door closed.");
  }
  
  printf("Notification invoked with: %d users\n", user_count);
  
  snprintf(x, 100, "%d DHCP Lease(s).\n%s", user_count, door);

  gtk_status_icon_set_tooltip(tray_icon, x);

#if LIBNOTIFY
  NotifyNotification *n;

  notify_init(name);
  n = notify_notification_new(name, x, NULL);
  //notify_notification_set_timeout(n, 10000);
  notify_notification_set_icon_from_pixbuf(n, gdk_pixbuf_from_pixdata(
                                              &icon_black_pixbuf, 
                                              true, 
                                              NULL));
  notify_notification_show(n, NULL);
  g_object_unref(G_OBJECT(n));
#endif
}

void set_status() {

  printf("Called set_status with: %d users\n", user_count);

  if (user_count < 1 && door_open == true) {
    gtk_status_icon_set_from_pixbuf(tray_icon, 
                                    gdk_pixbuf_from_pixdata(&icon_orange_pixbuf, 
                                    true, 
                                    NULL));
  } else if (user_count < 1 && door_open == false) {
    gtk_status_icon_set_from_pixbuf(tray_icon, 
                                    gdk_pixbuf_from_pixdata(&icon_red_pixbuf, 
                                    true, 
                                    NULL));
  } else if (user_count > 0 && door_open == true) {
    gtk_status_icon_set_from_pixbuf(tray_icon, 
                                    gdk_pixbuf_from_pixdata(&icon_yellow_pixbuf, 
                                    true, 
                                    NULL));
  } else if (user_count > 0 && door_open == false) {
    gtk_status_icon_set_from_pixbuf(tray_icon, 
                                    gdk_pixbuf_from_pixdata(&icon_black_pixbuf, 
                                    true, 
                                    NULL));
  }
}

bool do_it(bool force) {  

  printf("Called do_it with: %d users\n", user_count);
  
  get_bouncer_data();

  set_status();

  if (user_count > old_user_count || 
      user_count < old_user_count || 
      door_open != old_door_open ||
      force == true)
      
  {
    invoke_notification();
  }
  return true;
}

bool do_it_callback() {

  printf("Called do_it_callback with: %d users\n", user_count);
  
  do_it(false);
  
  return true;
}

int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  tray_icon = gtk_status_icon_new();
  g_signal_connect(G_OBJECT(tray_icon), "activate", 
                   G_CALLBACK(tray_icon_on_click), NULL);

  gtk_status_icon_set_visible(tray_icon, true);
  
  do_it(true);
  
  gtk_timeout_add(delay, (GtkFunction)do_it_callback, (gpointer)NULL);

  gtk_main();
  
  return 0;
}
