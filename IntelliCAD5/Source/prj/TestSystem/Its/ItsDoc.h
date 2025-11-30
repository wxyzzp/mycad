#pragma once
#include "ItsResult.h"
#include <vector>

class CItsDoc : public CDocument
{
protected:
	CItsDoc();
	DECLARE_DYNCREATE(CItsDoc)

public:
	virtual ~CItsDoc();

	void addCrash(CItsCrash*);
	void addFailure(CItsFailure*);
	void addMessage(CItsMessage*);

	const std::vector<CItsResult*>& getResults() const;

	void clear();

private:
	std::vector<CItsResult*> m_results;
};
