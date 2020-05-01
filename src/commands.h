#ifndef _COMMANDS_
#define _COMMANDS_

class cDescriptor;

class cParam
{
 public:
   cParam(){};
   ~cParam(){};

 public:
   const char *arguments;
};

class cCommand
{
 public:
   cCommand(void){};
   virtual ~cCommand(void){};

   virtual void Execute( cDescriptor &, cParam & )=0;
};

class cCommandsStack
{
 public:
   cCommandsStack(void);
   ~cCommandsStack(void);

 private:
   int m_CountCmd;
   struct sCommand {
      const char *name;
      const char *help;
      usINT level;
      cCommand *func;
   };
   class cParam param; 
   struct sCommand m_commands[MAX_SERVER_CMDS];

 public:
   static cCommandsStack *GetInstance();

   void Add(const char *n, const char *h, unsigned int level, cCommand *cmd);
   int Find(const char *cmd, int level, char *matches);
   void Help(cDescriptor &d, const char *cmd);
   bool Process_Command(cDescriptor *d, char *cmd);
};

#endif // _COMMANDS_
