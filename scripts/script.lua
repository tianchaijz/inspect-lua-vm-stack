local ta = {}
local tb = {}
ta.x = 1
ta.tb = tb
tb.ta = ta

local tc = {'a', [ta]='circle', x=1, 'b', {{'one', 'two'}, 'aa', 'bb', 'cc'}, 'd'}

print_table(ta)
print_table(tb)
print_table(tc)
