-- x86-64 GNU/Linux

function init()
   writeLine(".text")
   writeLine("  .global _start")
   writeLine("  .type _start, @function")
   writeLine("_start:")
end

function na()
end

codeCounter = 0
frameSize = 0
function receiveCode(opCode, param)
   codeCounter = codeCounter + 1
   writeLine(".Lcode" .. codeCounter .. ":")
   if opCode == Opcodes.Nop then
      writeLine("  nop")
   elseif opCode == Opcodes.Add then
      writeLine("  addb $" .. param .. ", (%r12)")
   elseif opCode == Opcodes.Next then
      writeLine("  add $" .. param .. ", %r12")
   elseif opCode == Opcodes.Bz then
      na()
   elseif opCode == Opcodes.B then
      writeLine("  jmp .Lcode" .. (codeCounter + param)  .. "(%rip)")
   elseif opCode == Opcodes.In then
      na()
   elseif opCode == Opcodes.Out then
      na()
   elseif opCode == Opcodes.Init then
      writeLine("  sub " .. param .. ", %rsp")
      frameSize = param
      writeLine("  mov %rsp, %r12")
   elseif opCode == Opcodes.Dumpstat then
      na()
   end
end

function atEnd()
   writeLine(".Lcode" .. codeCounter .. ":")
   writeLine("  add " .. frameSize .. ", %rsp")
   writeLine("  mov $60, %rax")
   writeLine("  mov $0, %rdi")
   writeLine("  syscall")
end