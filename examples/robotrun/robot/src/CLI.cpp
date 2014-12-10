/*
 * Copyright 2014 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * CLI.cpp
 *
 *  Created on: Dec 5, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include <Arduino.h>
#include "CLI.hpp"

Command::Command(const char commandSymbol, Command_actions_t * commandActions) :
	key(commandSymbol),
	next(NULL),
	actions(commandActions)
{

};

void Command::link(Command * nextCommand) {
	next = nextCommand;
};

bool Command::hasNext() {
	if (next == NULL) {
		return false;
	} else {
		return true;
	}
};

Command * Command::getNext() {
	return next;
};

bool Command::act(char symbol) {
	if (key == symbol) {
		if (actions != NULL) {
			actions();
		}
		return true;
	} else {
		return false;
	}
};


void CLI::Compute() {
	if ((currentCommand != NULL)
			|| (_commands == NULL)){
		return;
	}
	if (Serial.available()) {
	    // get the new byte:
	    char inChar = (char)Serial.read();
	    Command * c = _commands;
	    while(c != NULL) {
	    	currentCommand = c;
	    	if (c->act(inChar)) {
	    		break;
	    	} else {
	    		currentCommand = NULL;
	    		c = c->getNext();
	    	}
	    }
	}
};

void CLI::addAction(const char commandSymbol, Command_actions_t * actions) {
	Command * newCommand = new Command(commandSymbol,actions);
	Serial.print(commandSymbol); Serial.print(",");
	if (_commands == NULL) {
		_commands = newCommand;
		return;
	}
	Command * c = _commands;
	while(c != NULL) {
		if (c->hasNext()) {
			c = c->getNext();
		} else {
			c->link(newCommand);
			c = NULL;
		}
	}
};

void CLI::commandComplete() {
	currentCommand = NULL;
};
