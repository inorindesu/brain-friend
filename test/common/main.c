/* 
 * Copyright (C) 2013  Inori Sakura <inorindesu@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

void run_stack_test();
void run_opcodes_test();
void run_opcode_compiler_test();

int main()
{
  run_stack_test();
  run_opcodes_test();
  run_opcode_compiler_test();
  return 0;
}

