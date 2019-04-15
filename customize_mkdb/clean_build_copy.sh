target_path="/Users/liuziang/Documents/hxbb/server/game_server/gs/"
./clean.sh
echo "cleaned up"
cmake CMakeLists.txt&&make
cp -f ./libdb_x86_64.dylib libdb_x86_64.dylib $target_path
echo "copied to $target_path"