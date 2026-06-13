#ifndef APPROUTE_H
#define APPROUTE_H

#include <QString>

enum class AppRoute {
    Unknown = -1,
    Login,
    Register,
    ProfileSetup,
    Home,
    Analysis,
    Report,
    Profile,
    Payment,
    Admin
};

inline QString appRouteName(AppRoute route)
{
    switch (route) {
    case AppRoute::Login:
        return QStringLiteral("Login");
    case AppRoute::Register:
        return QStringLiteral("Register");
    case AppRoute::ProfileSetup:
        return QStringLiteral("ProfileSetup");
    case AppRoute::Home:
        return QStringLiteral("Home");
    case AppRoute::Analysis:
        return QStringLiteral("Analysis");
    case AppRoute::Report:
        return QStringLiteral("Report");
    case AppRoute::Profile:
        return QStringLiteral("Profile");
    case AppRoute::Payment:
        return QStringLiteral("Payment");
    case AppRoute::Admin:
        return QStringLiteral("Admin");
    case AppRoute::Unknown:
    default:
        return QStringLiteral("Unknown");
    }
}

#endif // APPROUTE_H
