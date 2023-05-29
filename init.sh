rm -rf include
rm -rf lib

SYSTEM=$(uname)

if [ "$SYSTEM" == "Linux" ]; then
    cp -R /home/linuxbrew/.linuxbrew/Cellar/glfw/3.3.8/include/GLFW .
    cp -R /home/linuxbrew/.linuxbrew/Cellar/glfw/3.3.8/lib .
elif [ "$SYSTEM" == "Darwin" ]; then
    cp -R /Users/$USER/goinfre/.brew/Cellar/glfw/3.3.8/include/GLFW .  #macos
    cp -R /Users/$USER/goinfre/.brew/Cellar/glfw/3.3.8/lib . # macos
fi

make re
