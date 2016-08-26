local ta = {}
local tb = {}
ta.x = 1
ta.tb = tb
tb.ta = ta

local tc = {'a', [ta]='circle', x=1, 'b', {{'one', 'two'}, 'aa', 'bb', 'cc'}, 'd'}

printTable(ta)
printTable(tb)
printTable(tc)
