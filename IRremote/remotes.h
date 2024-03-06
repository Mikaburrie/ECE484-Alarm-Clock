/*
 * remotes.h
 *
 *  Created on: Nov 5, 2014
 *      Author: John Convertino
 *      email: electrobs@gmail.com
 *
    Copyright (C) 2014 John Convertino

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef REMOTES_H_
#define REMOTES_H_

#include "irremote.h"

//all remotes must end with {something, NO_ACTION}

//ADAFRUIT_REMOTE
#define ADAFRUIT_ADDRESS 0xBF00
commandTemplate ADAFRUIT_REMOTE[] = {{0x02, VOLUME_UP}, {0x00, VOLUME_DOWN},
{0x01, PLAY_PAUSE}, {0x04, SETUP}, {0x05, PREV}, {0x06, STOP_MODE},
{0x08, CH_MINUS}, {0x09, ENTER_SAVE}, {0x0A, CH_PLUS}, {0x0C, ZERO_TEN},
{0x0D, NEXT}, {0x0E, REPEAT}, {0x10, ONE}, {0x11, TWO}, {0x12, THREE},
{0x14, FOUR}, {0x15, FIVE}, {0x16, SIX}, {0x18, SEVEN},
{0x19, EIGHT}, {0x1A, NINE}, {0xFF, NO_ACTION}};

//CARMP3_REMOTE
#define CARMP3_ADDRESS 0xFF00
commandTemplate CARMP3_REMOTE[] = {
   {0x45, CH_MINUS}, {0x46, CH}, {0x47, CH_PLUS}, 
   {0x44, PREV}, {0x40, NEXT}, {0x43, PLAY_PAUSE},
   {0x07, VOLUME_DOWN}, {0x15, VOLUME_UP}, {0x09, EQ},
   {0x16, ZERO_TEN}, {0x19, INPUT1}, {0x0D, INPUT2},
   {0x0C, ONE}, {0x18, TWO}, {0x5E, THREE},
   {0x08, FOUR}, {0x1C, FIVE}, {0x5A, SIX},
   {0x42, SEVEN}, {0x52, EIGHT}, {0x4A, NINE},
   {0xFF, NO_ACTION}
};

#endif /* REMOTES_H_ */
