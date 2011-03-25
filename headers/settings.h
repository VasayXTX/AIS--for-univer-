#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui>
#include <QtSql>
#include <QtWebKit>

#include "../headers/error.h"

namespace Settings{
    enum ETable{
        tCompany,
        tContract,
        tDeveloper,
        tProject,
        tProblem,
        tReport,
        tProblemDepend,
        tDistrProjDev
    };

    namespace Company{
        enum EIndex{
            iId = 0,
            iName = 1
        };
        extern unsigned int maxLenName;
    }

    namespace Project{
        enum EIndex{
            iId = 0,
            iName = 1,
            iDateBegin = 2,
            iDateEndExpect = 3,
            iIsComplete = 4
        };
        extern unsigned int maxLenName;
    }

    namespace Developer{
        enum EIndex{
            iLogin = 0,
            iPassword = 1,
            iFirstName = 2,
            iLastName = 3,
            iIsAdm = 4,
            iCompanyId = 5
        };
        extern unsigned int maxLenLogin;
        extern unsigned int maxLenPassword;
        extern unsigned int maxLenFirstName;
        extern unsigned int maxLenLastName;
        extern bool curIsAdm;
        extern QString curLogin;
    }

    namespace Problem{
        enum EIndex{
            iId = 0,
            iName = 1,
            iDeclaration = 2,
            iTimePlan = 3,
            iStatus = 4,
            iProjectId = 5
        };
        extern unsigned int maxLenName;
        extern unsigned int maxLenDeclaration;
        extern QVector<QString> vecStatus;
    }

    namespace Contract{
        enum EIndex{
            iNum = 0,
            iDateConclusion = 1,
            iStatus = 2,
            iCompanyId = 3,
            iProjectId = 4
        };
        extern QVector<QString> vecStatus;
    }

    namespace Report{
        enum EIndex{
            iId = 0,
            iDateBegin = 1,
            iDateEnd = 2,
            iDeclaration = 3,
            iDeveloperLogin = 4,
            iProblemId = 5
        };
        extern unsigned int maxLenDeclaration;
    }

    namespace ProblemDepend{
        enum EIndex{
            iId = 0,
            iProblem1Id = 1,
            iProblem2Id = 2
        };
    }

    namespace DistrProjDev{
        enum EIndex{
            iId = 0,
            iProjectId = 1,
            iDeveloperLogin = 2,
            iIsManager = 3
        };
    }

    namespace Database{
        extern QString configFile;
        extern QString fullName;
        extern QString datetimeSplit;
    }

    namespace WorkingWeek{
        extern unsigned int length;
        extern QTime tBegin;
        extern QTime tEnd;
        extern unsigned int weekendCount;
        extern unsigned int workingDayCount;
        extern QMap<unsigned int, unsigned int> vecDif;   //количество дней (значение) от выходного дня (ключ) до ближайшего рабочего
    }
}

#endif // SETTINGS_H
