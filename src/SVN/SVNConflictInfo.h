// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2016-2017, 2020 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

#include "SVNBase.h"
#include "SVNPrompt.h"
#include "ProgressDlg.h"
#include "TSVNPath.h"

struct svn_client_conflict_t;
struct svn_client_conflict_option_t;

/**
* \ingroup SVN
* Wrapper for the svn_client_conflict_option API.
*/
class SVNConflictOption
{
public:
    SVNConflictOption(const SVNConflictOption&) = delete;
    SVNConflictOption& operator=(SVNConflictOption&) = delete;

    svn_client_conflict_option_id_t GetId() const { return m_id; }
    CString GetLabel() const { return m_label; }
    CString GetDescription() const { return m_description; }
    operator svn_client_conflict_option_t *() const { return m_option; }
    int GetPreferredMovedTargetIdx() const { return m_preferred_moved_target_idx; }
    int GetPreferredMovedRelTargetIdx() const { return m_preferred_moved_reltarget_idx; }

    void SetMergedPropVal(const svn_string_t *propval);
    svn_error_t * SetMergedPropValFile(const CTSVNPath & filePath, apr_pool_t * pool);

protected:
    SVNConflictOption(svn_client_conflict_option_t *option,
                      svn_client_conflict_option_id_t id,
                      const CString & label,
                      const CString & description,
                      int preferred_moved_target_idx = -1,
                      int preferred_moved_reltarget_idx = -1);

private:
    svn_client_conflict_option_t *m_option;
    svn_client_conflict_option_id_t m_id;
    int m_preferred_moved_target_idx;
    int m_preferred_moved_reltarget_idx;
    CString m_label;
    CString m_description;

    friend class SVNConflictInfo;
};

/**
* \ingroup SVN
* Collection of SVNConflictOption.
*/
class SVNConflictOptions : public std::deque<std::unique_ptr<SVNConflictOption>>
{
public:
    SVNConflictOptions();
    ~SVNConflictOptions();

    apr_pool_t *GetPool() { return m_pool; }
    SVNConflictOption * FindOptionById(svn_client_conflict_option_id_t id);

private:
    apr_pool_t *m_pool;
};

/**
* \ingroup SVN
* Wrapper for the svn_client_conflict_t API.
*/
class SVNConflictInfo : public SVNBase
{
public:
    SVNConflictInfo();
    ~SVNConflictInfo();
    SVNConflictInfo(const SVNConflictInfo&) = delete;
    SVNConflictInfo& operator=(const SVNConflictInfo&) = delete;

    void SetProgressDlg(CProgressDlg * dlg) { m_pProgress = dlg; }

    bool Get(const CTSVNPath & path);
    CTSVNPath GetPath() const { return m_path; }
    // Wrapper for svn_client_conflict_get_operation()
    svn_wc_operation_t GetOperation() const;
    // Wrapper for svn_client_conflict_get_incoming_change()
    svn_wc_conflict_action_t GetIncomingChange() const;
    // Wrapper for svn_client_conflict_get_local_change().
    svn_wc_conflict_reason_t GetLocalChange() const;
    // Wrapper for svn_client_conflict_get_recommended_option_id().
    svn_client_conflict_option_id_t GetRecommendedOptionId() const;
    bool HasTreeConflict() const { return m_tree_conflicted != FALSE; }
    bool HasTextConflict() const { return m_text_conflicted != FALSE; }
    bool HasPropConflict() const { return m_prop_conflicts->nelts > 0; }
    bool IsBinary();
    int GetPropConflictCount() const;
    CString GetPropConflictName(int idx) const;
    bool GetPropValFiles(const CString & propertyName, CTSVNPath & mergedfile, CTSVNPath & basefile, CTSVNPath & theirfile, CTSVNPath & myfile);
    CString GetPropDiff(const CString & propertyName);
    bool GetTextContentFiles(CTSVNPath & basefile, CTSVNPath & theirfile, CTSVNPath & myfile);

    CString GetIncomingChangeSummary() const { return m_incomingChangeSummary; }
    CString GetDetailedIncomingChangeSummary() const { return m_detailedIncomingChangeSummary; }
    CString GetLocalChangeSummary() const { return m_localChangeSummary;  }
    CString GetPropDescription() const { return m_propDescription; }

    bool GetTreeResolutionOptions(SVNConflictOptions & result);
    bool GetTextResolutionOptions(SVNConflictOptions & result);
    bool GetPropResolutionOptions(SVNConflictOptions & result);
    operator svn_client_conflict_t *() { return m_conflict; }

    bool FetchTreeDetails();
protected:
    apr_pool_t *m_pool;
    apr_pool_t *m_infoPool;
    svn_client_conflict_t *m_conflict;
    CString m_incomingChangeSummary;
    CString m_detailedIncomingChangeSummary;
    CString m_localChangeSummary;
    CString m_propDescription;
    svn_boolean_t m_tree_conflicted;
    svn_boolean_t m_text_conflicted;
    apr_array_header_t *m_prop_conflicts;
    CTSVNPath m_path;
    SVNPrompt m_prompt;
    CProgressDlg *m_pProgress;

    static svn_error_t* cancelCallback(void *baton);
    static void notifyCallback(void *baton, const svn_wc_notify_t *notify, apr_pool_t * pool);
    svn_error_t * createPropValFiles(const char *propname, const char *mergedfile, const char *basefile, const char *theirfile, const char *myfile, apr_pool_t *pool);
};
