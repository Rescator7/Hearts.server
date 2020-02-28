#ifndef _COMMANDS_
#define _COMMANDS_

class cDescriptor;

class cParam
{
 public:
   cParam(){};
   ~cParam(){};

 public:
   const char * arguments;
// TODO: put parameters here
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
   int m_CountCmd; // count the number of commands installed
   struct sCommand {
      const char * name;
      cCommand * func;
   };
   class cParam param; 
   struct sCommand m_commands[MAX_SERVER_CMDS];

 public:
   static cCommandsStack* GetInstance();

   void Add(const char * commande, cCommand * cmd);
   bool Process_Command(cDescriptor * d, char * buffer);
};

#endif // _COMMANDS_
