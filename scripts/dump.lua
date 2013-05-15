
function init()
   writeLine("== START ==")
end

function receiveCode(opCode, param)
   writeLine("Opcode: " .. opCode .. ", Parameter: " .. param)
end

function atEnd()
   writeLine("== END ==")
end
