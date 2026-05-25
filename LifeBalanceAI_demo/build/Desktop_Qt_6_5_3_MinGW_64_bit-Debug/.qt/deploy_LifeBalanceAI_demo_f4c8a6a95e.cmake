include(D:/QtProjects/LifeBalanceAI/LifeBalanceAI_demo/build/Desktop_Qt_6_5_3_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/LifeBalanceAI_demo-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_ALL_MODULES_FOUND_VIA_FIND_PACKAGE "ZlibPrivate;EntryPointPrivate;Core;Gui;Widgets;Sql;Network")

qt6_deploy_runtime_dependencies(
    EXECUTABLE D:/QtProjects/LifeBalanceAI/LifeBalanceAI_demo/build/Desktop_Qt_6_5_3_MinGW_64_bit-Debug/LifeBalanceAI_demo.exe
    GENERATE_QT_CONF
)
