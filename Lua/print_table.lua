-- 非递归方式遍历表，并在遍历到每个节点时呼叫回调，表可以是循环引用的
--
-- theTable是要遍历的表
-- fnDoOnNode是在每个节点上的回调，其返回值控制是否终止遍历，原型如：function fnDoOnNode(nodeInfo, loopReference, referenceNodeInfo)
-- tableName是字符串表示的表名
--
-- 无返回值
function TraverseTable(theTable, fnDoOnNode, tableName)
    if type(theTable) ~= 'table' or type(fnDoOnNode) ~= 'function' then
        return
    end

    local stack = {}  -- 记录还未遍历节点的层级、键、值、父节点等信息
    local top = 1
    stack[top] = {1, tableName or 'root', theTable, nil}
    local tableNodes = {}  -- 记录已遍历表节点，以甑别表的循环引用

    -- 压栈记录未遍历的结点，并不断取栈顶遍历，直到栈空
    while top > 0 do
        local nodeInfo = stack[top]
        stack[top] = nil
        top = top - 1
        local currentValue = nodeInfo[3]
        if 'table' == type(currentValue) then
            -- 甑别循环引用
            local loopReference = false
            local referenceNodeInfo
            for i = 1, #tableNodes do
                if currentValue == tableNodes[i][3] then
                    loopReference = true
                    referenceNodeInfo = tableNodes[i]
                    break
                end
            end

            if loopReference then
                local stop = fnDoOnNode(nodeInfo, loopReference, referenceNodeInfo)
                if stop then
                    return
                end
            else
                local stop = fnDoOnNode(nodeInfo)
                if stop then
                    return
                end

                -- 将表的子节点入栈，以立即遍历
                for k, v in pairs(currentValue) do
                    top = top + 1
                    stack[top] = {nodeInfo[1]  + 1, k, v, nodeInfo}
                end

                table.insert(tableNodes, nodeInfo)  -- 记录已遍历的非循环引用的表节点
            end
        else
            local stop = fnDoOnNode(nodeInfo)
            if stop then
                return
            end
        end
    end
end

-- 打印表，格式如下：
-- testTable: table: 00448E70
--     testString: 这是一个测试
--     testFunction: function: 00B4DEA0
--     testNumber: 26
--     testSubTable: table: 00B4C710
--         testNumber: 26
--         testBoolean: true
--         testString: 这是一个测试
--         testFunction: function: 00B4ACA8
--     testLoopTable: table: 00448E70 [reference level: 1 name: testTable]
--     testBoolean: true
--     testLoopSubTable: table: 00448E98
--         testNumber: 26
--         testBoolean: true
--         testString: 这是一个测试
--         testFunction: function: 00447CF0
--         1: table: 00448E70 [reference level: 1 name: testTable]
--
-- theTable是要打印的表
-- tableName是字符串表示的表名，以供打印 
--
-- 无返回值
function PrintTable(theTable, tableName)
    local buffer = ''  -- 存储所有节点打印信息

    local function PrintNodeInfo(nodeInfo, loopReference, referenceNodeInfo)
        local nodeInfoStr = ''

        -- 根据节点层级进行空格缩进
        for i = 2, nodeInfo[1] do
            nodeInfoStr = nodeInfoStr .. '    '
        end

        -- 以冒号分隔节点键值
        nodeInfoStr = nodeInfoStr .. tostring(nodeInfo[2]) .. ': ' .. tostring(nodeInfo[3])

        -- 如果节点是循环引用节点，以中括号分隔所引用的节点信息，包括所引用节点层级和键路径
        if loopReference then
            local keyPath = {referenceNodeInfo[2]}
            local fatherNodeInfo = referenceNodeInfo[4]
            while fatherNodeInfo do
                table.insert(keyPath, fatherNodeInfo[2])
                fatherNodeInfo = fatherNodeInfo[4]
            end
            local temp = tostring(keyPath[#keyPath])
            for i = #keyPath - 1, 1, -1 do
                if type(keyPath[i]) ~= 'string' then
                    temp = temp .. '[' .. tostring(keyPath[i]) .. ']'
                else
                    temp = temp .. '.' .. tostring(keyPath[i])
                end
            end
            nodeInfoStr = nodeInfoStr .. ' [reference level: ' .. tostring(referenceNodeInfo[1]) .. ' name: ' .. temp .. ']'
        end

        buffer = buffer .. nodeInfoStr .. '\n'  -- 附加节点打印信息
    end

    TraverseTable(theTable, PrintNodeInfo, tableName)
    print(buffer)
end

-- 测试函数
function Test()
    local testTable = {}
    testTable.testString = '这是一个测试'
    testTable.testNumber = 26
    testTable.testBoolean = true
    testTable.testFunction = function () end
    testTable.testSubTable = {testString = '这是一个测试', testNumber = 26, testBoolean = true, testFunction = function () end}
    testTable.testLoopTable = testTable
    testTable.testLoopSubTable = {testString = '这是一个测试', testNumber = 26, testBoolean = true, testFunction = function () end, testTable}

    PrintTable(testTable, 'testTable')
    PrintTable(testTable.testLoopTable, 'testTable.testLoopTable')
    PrintTable(testTable.testLoopSubTable, 'testTable.testLoopSubTable')
end


Test()
