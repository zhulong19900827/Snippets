-- �ǵݹ鷽ʽ���������ڱ�����ÿ���ڵ�ʱ���лص����������ѭ�����õ�
--
-- theTable��Ҫ�����ı�
-- fnDoOnNode����ÿ���ڵ��ϵĻص����䷵��ֵ�����Ƿ���ֹ������ԭ���磺function fnDoOnNode(nodeInfo, loopReference, referenceNodeInfo)
-- tableName���ַ�����ʾ�ı���
--
-- �޷���ֵ
function TraverseTable(theTable, fnDoOnNode, tableName)
    if type(theTable) ~= 'table' or type(fnDoOnNode) ~= 'function' then
        return
    end

    local stack = {}  -- ��¼��δ�����ڵ�Ĳ㼶������ֵ�����ڵ����Ϣ
    local top = 1
    stack[top] = {1, tableName or 'root', theTable, nil}
    local tableNodes = {}  -- ��¼�ѱ�����ڵ㣬��굱���ѭ������

    -- ѹջ��¼δ�����Ľ�㣬������ȡջ��������ֱ��ջ��
    while top > 0 do
        local nodeInfo = stack[top]
        stack[top] = nil
        top = top - 1
        local currentValue = nodeInfo[3]
        if 'table' == type(currentValue) then
            -- 굱�ѭ������
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

                -- ������ӽڵ���ջ������������
                for k, v in pairs(currentValue) do
                    top = top + 1
                    stack[top] = {nodeInfo[1]  + 1, k, v, nodeInfo}
                end

                table.insert(tableNodes, nodeInfo)  -- ��¼�ѱ����ķ�ѭ�����õı�ڵ�
            end
        else
            local stop = fnDoOnNode(nodeInfo)
            if stop then
                return
            end
        end
    end
end

-- ��ӡ����ʽ���£�
-- testTable: table: 00448E70
--     testString: ����һ������
--     testFunction: function: 00B4DEA0
--     testNumber: 26
--     testSubTable: table: 00B4C710
--         testNumber: 26
--         testBoolean: true
--         testString: ����һ������
--         testFunction: function: 00B4ACA8
--     testLoopTable: table: 00448E70 [reference level: 1 name: testTable]
--     testBoolean: true
--     testLoopSubTable: table: 00448E98
--         testNumber: 26
--         testBoolean: true
--         testString: ����һ������
--         testFunction: function: 00447CF0
--         1: table: 00448E70 [reference level: 1 name: testTable]
--
-- theTable��Ҫ��ӡ�ı�
-- tableName���ַ�����ʾ�ı������Թ���ӡ 
--
-- �޷���ֵ
function PrintTable(theTable, tableName)
    local buffer = ''  -- �洢���нڵ��ӡ��Ϣ

    local function PrintNodeInfo(nodeInfo, loopReference, referenceNodeInfo)
        local nodeInfoStr = ''

        -- ���ݽڵ�㼶���пո�����
        for i = 2, nodeInfo[1] do
            nodeInfoStr = nodeInfoStr .. '    '
        end

        -- ��ð�ŷָ��ڵ��ֵ
        nodeInfoStr = nodeInfoStr .. tostring(nodeInfo[2]) .. ': ' .. tostring(nodeInfo[3])

        -- ����ڵ���ѭ�����ýڵ㣬�������ŷָ������õĽڵ���Ϣ�����������ýڵ�㼶�ͼ�·��
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

        buffer = buffer .. nodeInfoStr .. '\n'  -- ���ӽڵ��ӡ��Ϣ
    end

    TraverseTable(theTable, PrintNodeInfo, tableName)
    print(buffer)
end

-- ���Ժ���
function Test()
    local testTable = {}
    testTable.testString = '����һ������'
    testTable.testNumber = 26
    testTable.testBoolean = true
    testTable.testFunction = function () end
    testTable.testSubTable = {testString = '����һ������', testNumber = 26, testBoolean = true, testFunction = function () end}
    testTable.testLoopTable = testTable
    testTable.testLoopSubTable = {testString = '����һ������', testNumber = 26, testBoolean = true, testFunction = function () end, testTable}

    PrintTable(testTable, 'testTable')
    PrintTable(testTable.testLoopTable, 'testTable.testLoopTable')
    PrintTable(testTable.testLoopSubTable, 'testTable.testLoopSubTable')
end


Test()
