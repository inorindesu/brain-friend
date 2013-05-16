
-- init and atEnd are not necessary.

function receiveCode(opCode, param)
   opCodeString = ""
   if     opCode == Opcodes.Nop then opCodeString = "nop"
   elseif opCode == Opcodes.Add then opCodeString = "add"
   elseif opCode == Opcodes.Next then opCodeString = "next"
   elseif opCode == Opcodes.Bz then opCodeString = "bz"
   elseif opCode == Opcodes.B then opCodeString = "b"
   elseif opCode == Opcodes.In then opCodeString = "in"
   elseif opCode == Opcodes.Out then opCodeString = "out"
   elseif opCode == Opcodes.Init then opCodeString = "init"
   elseif opCode == Opcodes.Dumpstat then opCodeString = "dumpstat"
   else opCodeString = "(N/A)"
   end
   
   if opCode == Opcodes.Nop or opCode == Opcodes.Dumpstat
      or opCode == Opcodes.In or opCode == Opcodes.Out then
      writeLine(opCodeString)
   else
      writeLine(opCodeString .. " "  .. param)
   end
end
