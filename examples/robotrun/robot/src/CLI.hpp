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
 * CLI.hpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef CLI_HPP_
#define CLI_HPP_

typedef void Command_actions_t();

class Command {
	private:
		const char key;
		Command * next;
		Command_actions_t * actions;
	public:
		Command(const char commandSymbol, Command_actions_t * actions);
		void link(Command * nextCommand);
		bool hasNext();
		Command * getNext();
		bool act(char symbol);
};

class CLI {
	private:
		Command * _commands;
		Command * currentCommand;
	public:
		CLI() :
			_commands(NULL),
			currentCommand(NULL) {};
		void Compute();
		void addAction(const char commandSymbol, Command_actions_t * actions);
		void commandComplete();
};


#endif /* CLI_HPP_ */
