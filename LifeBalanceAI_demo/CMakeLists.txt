cmake_minimum_required(VERSION 3.19)
project(LifeBalanceAI_demo LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Network)

qt_standard_project_setup()

qt_add_executable(LifeBalanceAI_demo
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasemanager.cpp
    databasemanager.h
    aimanager.cpp
    aimanager.h
    feedbackdialog.cpp
    feedbackdialog.h
    deepanalysisdialog.cpp
    deepanalysisdialog.h
    animateddialog.cpp
    animateddialog.h
    bottomnavbar.cpp
    bottomnavbar.h
    circularprogressbar.cpp
    circularprogressbar.h
    trendlinechart.cpp
    trendlinechart.h
    calendargridview.cpp
    calendargridview.h
    completionfeedback.cpp
    completionfeedback.h
    designtokens.h
    motionhelper.cpp
    motionhelper.h
    thememanager.cpp
    thememanager.h
    loadingoverlay.cpp
    loadingoverlay.h
    sidedrawer.cpp
    sidedrawer.h
    uifactory.cpp
    uifactory.h
    welcomeoverlay.cpp
    welcomeoverlay.h
    models/dto.h
    services/checkinservice.cpp
    services/checkinservice.h
    services/freezecardservice.cpp
    services/freezecardservice.h
    services/deepanalysisservice.cpp
    services/deepanalysisservice.h
    services/reportservice.cpp
    services/reportservice.h
    services/userservice.cpp
    services/userservice.h
)

qt_add_resources(LifeBalanceAI_demo "LifeBalanceResources"
    PREFIX "/"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    FILES
        resources/style.qss
        resources/assets/welcome_hero.png
        resources/assets/welcome_success.png
        resources/assets/app_logo.png
        resources/assets/empty_plan.png
        resources/assets/empty_report.png
        resources/assets/ai_analysis.png
        resources/assets/profile_badge.png
        resources/assets/icon_diet.png
        resources/assets/icon_exercise.png
        resources/assets/icon_feedback.png
        resources/assets/icon_report.png
        resources/fonts/MiSans-Regular.otf
        resources/fonts/MiSans-Medium.otf
        resources/fonts/MiSans-Bold.otf
        resources/fonts/SmileySans-Oblique.otf
)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.env")
    qt_add_resources(LifeBalanceAI_demo "LifeBalanceLocalConfig"
        PREFIX "/config"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES
            .env
    )
endif()

target_include_directories(LifeBalanceAI_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(LifeBalanceAI_demo
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
        Qt6::Network
)

if (ANDROID)
    set(ANDROID_OPENSSL_LIB_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/android_openssl/ssl_3/${CMAKE_ANDROID_ARCH_ABI}")
    set_property(TARGET LifeBalanceAI_demo PROPERTY QT_ANDROID_EXTRA_LIBS
        "${ANDROID_OPENSSL_LIB_DIR}/libcrypto_3.so"
        "${ANDROID_OPENSSL_LIB_DIR}/libssl_3.so"
    )
endif()

include(GNUInstallDirs)

install(TARGETS LifeBalanceAI_demo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    resources/export_report.py
    resources/style.qss
    DESTINATION resources
)

install(DIRECTORY
    resources/assets
    resources/fonts
    resources/licenses
    DESTINATION resources
)

qt_generate_deploy_app_script(
    TARGET LifeBalanceAI_demo
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
