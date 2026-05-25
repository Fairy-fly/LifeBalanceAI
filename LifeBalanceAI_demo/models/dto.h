#ifndef MODELS_DTO_H
#define MODELS_DTO_H

#include <QString>
#include <QDate>
#include <QVector>

namespace LifeBalanceAI {
namespace Models {

struct UserInfo {
    int     uid            = 0;
    QString phone;
    QString role;
    int     streakDays     = 0;
    int     freezeCards    = 0;
    int     graceDays      = 0;
    int     totalCheckins  = 0;
    QDate   createdAt;
};

struct ProfileData {
    int     uid      = 0;
    QString nickname;
    int     age      = 0;
    double  height   = 0.0;
    double  weight   = 0.0;
    QString gender;
    QString goal;
    QString allergy;
    QString dietPref;
    QString sportPref;
};

struct PlanItem {
    int     itemId   = 0;
    int     planId   = 0;
    QString date;
    QString timeSlot;
    QString content;
    bool    isDone   = false;
};

struct FeedbackData {
    int     fid       = 0;
    int     uid       = 0;
    int     itemId    = 0;
    QString content;
    QString imagePath;
    QString date;
    QString time;
};

struct RecipeItem {
    QString name;
    QString reason;
    int     calories = 0;
};

struct DeepAnalysisResult {
    QString nutritionProtein;
    QString nutritionCarbs;
    QString nutritionFat;
    QString nutritionAdvice;
    QString exerciseChange;
    QString exerciseConfidence;
    QVector<RecipeItem> recipes;
    QString lazySlot;
    QString lazyAdvice;
};

struct ReportData {
    int     rid        = 0;
    int     uid        = 0;
    QString type;
    QString startDate;
    QString endDate;
    QString aiSummary;
    QString exportPath;
    QString posterPath;
    QString createdAt;
};

struct PlanGenerationResult {
    QString longTermWeek;
    QString longTermMonth;
    QString longTermYear;
    QVector<PlanItem> dailyItems;
};

struct CheckinResult {
    bool    checkedIn        = false;
    bool    streakBroken     = false;
    int     newStreak        = 0;
    bool    reprofileRequired = false;
    QString message;
};

struct StreakInfo {
    int streakDays    = 0;
    int totalCheckins = 0;
    int freezeCards   = 0;
    int graceDays     = 0;
};

struct FreezeResult {
    bool streakProtected = false;
    int  remainingCards  = 0;
    int  graceDaysUsed   = 0;
    bool streakLost      = false;
};

} // namespace Models
} // namespace LifeBalanceAI

#endif // MODELS_DTO_H