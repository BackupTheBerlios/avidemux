// $Id: jogshuttle.h,v 1.12 2007/02/20 06:32:05 ddennedy Exp $

/*
 * Copyright (C) 2001 Tomoaki Hayasaka <hayasakas@postman.riken.go.jp>
 * Copyright (C) 2001-2007 Dan Dennedy <dan@dennedy.org>
 * Taken from Kino 1.1.1, with permission, and hacked severely by
 * Chris MacGregor <chris-avidemux@bouncingdog.dom>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef JOGSHUTTLE_H
#define JOGSHUTTLE_H

#include <map>

#include <gtk/gtk.h>
#ifdef USE_JOG
#include "mediactrl.h"

/** 
 * Type for JogShuttle event callbacks.
 *
 * The callback system is mainly used by the preference dialog to get
 * a notification when an event is received, instead of the usual
 * case, which is to perform a command. */

typedef void (* JogShuttleCallback) (void *, unsigned short, unsigned short);

/**
 * Class to encapsulate an abstract JogShuttle device.
 * 
 * This class is used by the kino core to interact with a JogShuttle
 * device.  (Also used by the preference dialog for configuration).
 * It depends on the GDK input system, and uses the libmediactrl stuff
 * to translate different (more or less broken) devices events into
 * something that resembles the linux event system.  It also keeps
 * track of modifier keys, that is, up to two keys pressed in succession. */

class PhysicalJogShuttle
{ 
private: 
    /** Singleton pattern. */
    static PhysicalJogShuttle *_instance; 
    /** Placeholder for callbacks. */
    JogShuttleCallback _callback; 
    /** Any client supplied callbackdata is here. */
    void * _callbackdata;

protected:
    static void inputCallback (gpointer data, gint source,
                               GdkInputCondition condition);
    void inputCallback (gint source, GdkInputCondition condition);
    PhysicalJogShuttle();

public:
    ~PhysicalJogShuttle();
    bool start();
    void stop();
    static PhysicalJogShuttle & getInstance();
    void registerCallback (void * user, JogShuttleCallback callback);
    void deregisterCallback();
    struct media_ctrl_key *getKeyset();

private:
    void jog (int dir);
    void shuttle (gfloat angle);
    void button (struct media_ctrl_event *);
    void button_old (int code);

    typedef std::map <unsigned short, Action> ButtonActionMap;
    typedef ButtonActionMap::iterator ButtonActionIter;

    struct media_ctrl _ctrl;
    int input_;
    gint monitorTag_;

    /* Used to keep track of "modifier" keys - only the buttons
       can be used, only the code is used */
    unsigned short _modifier_code;
//    media_ctrl_key *_modifier;
    ButtonActionMap button_action_map;
    bool show_buttons;
};
#endif
#endif
