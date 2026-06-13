#ifndef AIRESPONSEPARSER_H
#define AIRESPONSEPARSER_H

#include <QString>
#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class AiResponseParser
{
public:
    static Models::AiJsonParseResult parseJsonObject(const QString &raw);
    static Models::DeepAnalysisParseResult parseDeepAnalysis(const QString &raw);
    static Models::ReportSummaryParseResult parseReportSummary(const QString &raw);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // AIRESPONSEPARSER_H
