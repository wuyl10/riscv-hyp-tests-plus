# 定义源文件和目标文件夹的路径
source_file="test_register.c"
source_dir="./build/"
target_dir="./output/"

# 从代码段中提取未注释的 TEST_REGISTER 名称
selected_test=$(grep -oP 'TEST_REGISTER\(\K[^)]+' <(grep -v '^ *//' $source_file))

# 检查是否提取到未注释的 TEST_REGISTER 名称
if [ -z "$selected_test" ]; then
    echo "No uncommented TEST_REGISTER found."
    exit 1
fi

# 注释当前未注释的 TEST_REGISTER
sed -i "s/^\([^\/]*TEST_REGISTER($selected_test)\)/\/\/ \1/" $source_file

# 找到需要取消注释的 TEST_REGISTER
previous_test=$(grep -B1 "TEST_REGISTER($selected_test)" $source_file | head -n 1 | sed -n 's/^[ \t]*\/\/[ \t]*TEST_REGISTER(\([^)]*\)).*/\1/p')

# 取消注释上一条注释的 TEST_REGISTER
if [ -n "$previous_test" ]; then
    sed -i "s/^[ \t]*\/\/[ \t]*\(TEST_REGISTER($previous_test)\)/\1/" $source_file
else
    echo "No previous commented TEST_REGISTER found."
    exit 1
fi