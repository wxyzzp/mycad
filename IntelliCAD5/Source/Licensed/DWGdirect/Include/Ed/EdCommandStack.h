#ifndef _ODEDCOMMANDSTACK_H_INCLUDED_
#define _ODEDCOMMANDSTACK_H_INCLUDED_

/*
#define ODRX_CMD_MODAL            0x00000000
#define ODRX_CMD_TRANSPARENT      0x00000001
#define ODRX_CMD_USEPICKSET       0x00000002
#define ODRX_CMD_REDRAW           0x00000004
#define ODRX_CMD_NOPERSPECTIVE    0x00000008  // NOT allowed in perspective views
#define ODRX_CMD_NOMULTIPLE       0x00000010
#define ODRX_CMD_NOTILEMODE       0x00000020  // NOT allowed with TILEMODE == 1
#define ODRX_CMD_NOPAPERSPACE     0x00000040  // NOT allowed in Paperspace
#define ODRX_CMD_PLOTONLY         0x00000080  // Allowed in plot-only state
#define ODRX_CMD_NOOEM            0x00000100
#define ODRX_CMD_UNDEFINED        0x00000200
#define ODRX_CMD_INPROGRESS       0x00000400
#define ODRX_CMD_DEFUN            0x00000800
#define ODRX_CMD_NONEWSTACK       0x00010000  //For internal use only
#define ODRX_CMD_NOINTERNALLOCK   0x00020000
#define ODRX_CMD_DOCREADLOCK      0x00080000  // not set = DOCWRITELOCK
#define ODRX_CMD_DOCEXCLUSIVELOCK 0x00100000  // not set = DOCSHAREDLOCK
#define ODRX_CMD_SESSION          0x00200000  // Run cmd handler in the session fiber
#define ODRX_CMD_INTERRUPTIBLE    0x00400000  // Supports OPM display of command properties
#define ODRX_CMD_NOHISTORY        0x00800000  // Command does not become default
*/


#define ODRX_MAX_CMDNAME_LEN      64

#define ODRX_COMMAND_DOCK      "ACAD_REGISTERED_COMMANDS"

typedef void (*OdRxFunctionPtr) ();

class OdEdCommand;
class OdEdCommandContext;

#include "RxObject.h"

class OdRxIterator;
typedef OdSmartPtr<OdRxIterator> OdRxIteratorPtr;

class OdRxDictionary;
class OdEdUIContext;

#include "DD_PackPush.h"

/** Description:
    Class that allows custom DWGdirect applications to register command objects that can be 
    executed from within a DWGdirect client application.

    Remarks:
    A custom command can be created by deriving a class from OdEdCommand, and overriding
    the pure virtual functions in this class. It can then be registered with the global command
    stack by calling OdEdCommandStack::addCommand() on the global stack (which can 
    be obtained using the odedRegCmds function).

    {group:OdEd_Classes}
*/
class FIRSTDLL_EXPORT OdEdCommand : public OdRxObject 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEdCommand);

  /** Description:
      Returns the group name of this command. This name is used to group sets of commands
      within the global command stack.
  */
  virtual const OdString  groupName() const = 0;

  /** Description:
      Returns the global name of this command (non-localized), which must be unique among
      global names of registered commands.
  */
  virtual const OdString  globalName() const = 0;

  /** Description:
      Returns the localized name of this command, which must be unique among 
      local names of registered commands.
  */  
  virtual const OdString  localName() const = 0;

  /** Description:
      This function is called to execute this command object, or in other words the client's
      override of this function contains the body of the command.
      
      Arguments:
      pCmdCtx (I) Context object that can be used to interact with the calling 
              application as part of an interactive command.  
              
      See Also:
      OdEdCommandContext::userIO
  */
  virtual void            execute(OdEdCommandContext* pCmdCtx) = 0;

  /*
  virtual void            *commandApp() const = 0;
  virtual void            commandUndef(bool undefIt) = 0;
  virtual OdInt32         commandFlags() const = 0;
  virtual OdEdUIContext   *UIContext() const = 0;
  virtual int             functionCode() const = 0;
  virtual void            functionCode(int fcode) = 0;
  virtual const HINSTANCE resourceHandle() const = 0;
  */
};

typedef OdSmartPtr<OdEdCommand> OdEdCommandPtr;


/** Description:

    {group:Other_Classes}
*/
class FIRSTDLL_EXPORT OdEdCommandStackReactor : public OdRxObject 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEdCommandStackReactor);
  
  virtual void commandAdded        (OdEdCommand* pCmd);
  virtual void commandWillBeRemoved(OdEdCommand* pCmd);
  virtual void commandWillStart    (OdEdCommand* pCmd);
  virtual void commandEnded        (OdEdCommand* pCmd);
  virtual void commandCancelled    (OdEdCommand* pCmd);
  virtual void commandFailed       (OdEdCommand* pCmd);
};

typedef OdSmartPtr<OdEdCommandStackReactor> OdEdCommandStackReactorPtr;


/** Description:
    Class used to store a set of custom commands, and provide access to these commands.

    {group:OdEd_Classes}
*/
class FIRSTDLL_EXPORT OdEdCommandStack : public OdRxObject 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEdCommandStack);

  /** Description:
      Replace the global command stack with the passed in value. Note that the global command
      stack is initialized automatically by DWGdirect when odInitialize is called.  This 
      function is intended for users who want to replace the existing global command stack with 
      a new version.
  */
  static void initialize(OdEdCommandStack* pStack = OdEdCommandStack::createObject());

  /** Description:
      Remove the current global command stack from the DWGdirect system registry.
  */
  static void uninitialize();

  virtual void addReactor(OdEdCommandStackReactor* pReactor) = 0;

  virtual void removeReactor(OdEdCommandStackReactor* pReactor) = 0;

  /** Description:
      Adds a specified command to this command stack.
  */
  virtual void addCommand(OdEdCommand* pCmd) = 0;

  /** Description:
      Returns an iterator that iterates through all OdEdCommand objects in this stack, 
      by global name.
  */
  virtual OdRxIteratorPtr newIterator() = 0;

  /** Description:
      Returns an iterator that iterates through all group objects in this stack,
      where each group is an OdRxDictionary object.
  */
  virtual OdRxIteratorPtr newGroupIterator() = 0;

  virtual void popGroupToTop(const OdString& cmdGroupName) = 0;

  enum LookupFlags
  {
    kGlobal       = 0x01,
    kLocal        = 0x02,
    kTopGroupOnly = 0x04
  };
  /** Description:
      Returns a smart pointer to the OdEdCommand object with the specified name within
      this stack.

      Arguments:
      cmdName (I) Name of the command to find.
      globalFlag (I) If true, treat cmdName as a global name, otherwise as a local name.

      Return Value:
      A smart pointer to the specified OdEdCommand object if found, otherwise a null smart pointer.
  */
  virtual OdEdCommandPtr lookupCmd(const OdString& cmdName, int lookupFlags = kGlobal|kLocal) = 0;

  /** Description:
      Executes specified command. Sends notifications to OdEdCommandStackReactor's.

      Arguments:
      pCmd (I) Pointer to command object to execute.
      pCmdCtx (I) Context object passed to command object's execute function 
        (see OdEdCommand::execute).
  */
  virtual void executeCommand(OdEdCommand* pCmd, OdEdCommandContext* pCmdCtx = 0) = 0;

  /** Description:
      Removes the command with the specified group name and global name from this stack.

      Remarks:
      If the specified command is not found, this function does nothing.
  */
  virtual void removeCmd(const OdString& cmdGroupName, const OdString& cmdGlobalName) = 0;

  /** Description:
      Removes the group of commands with the specified group name from this stack (the group
      as well as all commands in the group are removed).

      Remarks:
      If the specified group name is not found, this function does nothing.
  */
  virtual void removeGroup(const OdString& groupName) = 0;
};

typedef OdSmartPtr<OdEdCommandStack> OdEdCommandStackPtr;

/** Description:
    Returns the global command stack.
*/
FIRSTDLL_EXPORT OdEdCommandStackPtr odedRegCmds();


#include "DD_PackPop.h"

#endif //#ifndef _ODEDCOMMANDSTACK_H_INCLUDED_

