make PLAT=spike LOG_LEVEL=4  CROSS_COMPILE=riscv64-unknown-elf-
make PLAT=nemu LOG_LEVEL=4  CROSS_COMPILE=riscv64-unknown-elf-

# 从代码段中提取未注释的 TEST_REGISTER 名称
selected_test=$(grep -oP 'TEST_REGISTER\(\K[^)]+' <(grep -v '^ *//' test_register.c))

# 定义源文件夹和目标文件夹的路径
source_dir="./build/"
target_dir="./output/"

# 定义统一的文件名
file_name="rvh_test"
des_name="$selected_test"

# 复制并重命名 bin 文件
cp "${source_dir}nemu/${file_name}.bin" "${target_dir}nemu+bin/${des_name}.bin"


# 复制并重命名 elf 文件
cp "${source_dir}spike/${file_name}.elf" "${target_dir}spike+elf/${des_name}.elf"


# 复制并重命名 asm 文件
cp "${source_dir}nemu/${file_name}.asm" "${target_dir}nemu+bin/${des_name}.asm"
cp "${source_dir}spike/${file_name}.asm" "${target_dir}spike+elf/${des_name}.asm"



#############注释test_register.c中测试点###################

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