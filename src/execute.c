/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"
#include "deque.h"
#include <fcntl.h>
#include <stdio.h>

#include "quash.h"

int previousRead = -2;
int Mypipe[2];
bool firstTime = true;
IMPLEMENT_DEQUE_STRUCT(pidQueue, pid_t);
IMPLEMENT_DEQUE(pidQueue, pid_t);

typedef struct Job {
  pidQueue pidQ;
  int Id;
  char* command;
} Job;

IMPLEMENT_DEQUE_STRUCT(jobQueue, Job);
IMPLEMENT_DEQUE(jobQueue, Job);

jobQueue jobQ;

// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)

/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  char *curnt_dir =  NULL;
  curnt_dir = getwd(curnt_dir);
  // Change this to true if necessary
  *should_free = false;

  return curnt_dir;
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();
  return getenv(env_var);
  // TODO: Remove warning silencers
  //(void) env_var; // Silence unused variable warning

  //return "???";
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  IMPLEMENT_ME();


  //run_pwd();

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // TODO: Remove warning silencers
  (void) exec; // Silence unused variable warning
  (void) args; // Silence unused variable warning

  // TODO: Implement run generic
  execvp(exec, args);
  //IMPLEMENT_ME();

  perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  char** str = cmd.args;

  // TODO: Remove warning silencers
  //(void) str; // Silence unused variable warning
  while(*str != NULL)
  {
    printf("%s ", *str);
    str++; //next string
  }

  printf("\n");
  // TODO: Implement echo
  //IMPLEMENT_ME();

  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Remove warning silencers
  (void) env_var; // Silence unused variable warning
  (void) val;     // Silence unused variable warning
  setenv(env_var, val, 1);
  // TODO: Implement export.
  // HINT: This should be quite simple.
  IMPLEMENT_ME();
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;

  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }

  // TODO: Change directory
  chdir(dir);
  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
  //IMPLEMENT_ME();
  setenv("OLD_PWD", getenv("PWD"), 1);
  setenv("PWD", dir, 1);
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  (void) signal; // Silence unused variable warning
  (void) job_id; // Silence unused variable warning

  // TODO: Kill all processes associated with a background job
  IMPLEMENT_ME();
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  bool should_free = false;
  fprintf(stdout,"%s \n", get_current_directory(&should_free));

  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  IMPLEMENT_ME();

  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, Job* aJob) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true
  //bool pause = true;
  // TODO: Remove warning silencers
  (void) p_in;  // Silence unused variable warning
  (void) p_out; // Silence unused variable warning
  (void) r_in;  // Silence unused variable warning
  (void) r_out; // Silence unused variable warning
  (void) r_app; // Silence unused variable warning

  // TODO: Setup pipes, redirects, and new process
  //IMPLEMENT_ME();

  if(p_out)
  {
    pipe(Mypipe);
  }

  pid_t pid = fork(); 

  push_back_pidQueue(&aJob->pidQ, pid);
  if(pid < 0)
  {
    printf("Error occurred, fork fail");
    return;
  }
  else if(pid == 0) //child
  {
    //while(pause);
    if(p_in)
    {
      dup2(previousRead, STDIN_FILENO); //duplicate read end
    }
    if(p_out)
    {
      dup2(Mypipe[1], STDOUT_FILENO); //duplicate write end
    }

    if(r_in)
    {
      FILE* file_inp = freopen(holder.redirect_in, "r", stdin);
      if(file_inp == NULL)
      {
        printf("Unable to read file");
      }
    }
    if(r_out)
    {
      FILE * file_outp;
      if(r_app)
      {
        file_outp = fopen(holder.redirect_out, "a"); 
      }
      else
      {
        file_outp = fopen(holder.redirect_out, "w"); 
      }
      dup2(fileno(file_outp), STDOUT_FILENO);
      fclose(file_outp);
    }
    child_run_command(holder.cmd);
    exit(EXIT_SUCCESS);
  }
  else //parent
  {
    push_back_pidQueue(&aJob->pidQ, pid);
    parent_run_command(holder.cmd); 
    if( p_out )
    {
        previousRead = Mypipe[0];
        close(Mypipe[1]);
    }
  }
  printf("p_in: %d p_out: %d r_in %d r_out %d\n", p_in, p_out, r_in, r_out);
  //parent_run_command(holder.cmd); // This should be done in the parent branch of
                                  // a fork
  //child_run_command(holder.cmd); // This should be done in the child branch of a fork
}

// Run a list of commands
void run_script(CommandHolder* holders) {

  if(firstTime)
  {
    jobQ = new_jobQueue(1);
    firstTime = false;
  }

  if (holders == NULL)
    return;

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;
  Job curnt_Job;
  curnt_Job.pidQ = new_pidQueue(1);
  curnt_Job.command = get_command_string();
  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i], &curnt_Job);

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    while(!is_empty_pidQueue(&curnt_Job.pidQ))
    {
      int status = 0;
      waitpid(pop_front_pidQueue(&curnt_Job.pidQ), &status, 0);
    }
    destroy_pidQueue(&curnt_Job.pidQ);
  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    if(is_empty_jobQueue(&jobQ))
    {
      curnt_Job.Id = 1;
    }
    else
    {
      curnt_Job.Id = peek_back_jobQueue(&jobQ).Id + 1;
    }
    push_back_jobQueue(&jobQ, curnt_Job);
    // TODO: Once jobs are implemented, uncomment and fill the following line
    print_job_bg_start(curnt_Job.Id, peek_front_pidQueue(&curnt_Job.pidQ), curnt_Job.command);
  }
}
