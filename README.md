## **MiniProject-1**
## Part-A: Basic System Calls

### Specification 1
#### - ```gethostname()``` is used to get the hostname of the system.
#### - To get system name, I used ```passwd``` struct in ```<pwd.h>``` header file.
### Specification 2
#### - ```strtok()``` is used to tokenize based on ```";"``` and ```" "```.
#### - tokenizing for ```"&"``` is done manually in ```system.c``` file.
### Specification 3
#### - ```getcwd()``` is used to get the current working directory.
#### - Relative path is handled properly by storing the path of the source file directory.
#### - ```chdir()``` is used to change the current working directory.
#### - Multiple arguments are also handled by using ```strtok()``` in a loop and storing all the locations to hop to.
### Specification 4
#### - Flags ```-l``` and ```-a``` and all combinations  with repititions are handled.
#### - Structures ```stat``` and ```dirent``` are used.
#### - ```stat()``` is used to get the file information(file type and permissions).
#### - ```qsort()``` is used to sort in alphabetical order.
#### - Aliases ```~```, ```.```, ```..``` and ```-``` are handled appropriately.
### Specification 5
#### - Commands stored in an array. Log array is loaded at the start of the shell.
#### - As array is updated, file is updated.
#### - To purge, file is overwritten. To execute, command is retrieved from the array and executed.
### Specification 6
#### - Commands are tokenized based on ```"&"``` and stored in an array.
#### - Executed using execvp(). ```time.h``` library used to check whether foreground process exceeds 2 seconds.
#### - For background processes, they are added to process table, to store and retrieve them later. (to know the name of the process based on PID)
#### - Completion of background process and printing the notification is done by signal handler function.
### Specification 7
#### - ```/proc``` directory is used to get the information of the process.
#### - The contents of ```/proc/PID/stat``` are parsed to get the required information.
#### - Prints absolute path of the process executable(for that section)
### Specification 8
#### - only two flags which are space separated may be given. That is, -fe, -de.. such cannot be given.
#### - Again, ```dirent``` and ```stat``` structures are used.
#### - Mentioned directory is recursively searched for the matching name, and the path is printed based on the color scheme.
## Part-B: Processes, Files and Misc. (Assumptions)
### myshrc.txt
#### - For this part, I have used a file named ```myshrc.txt``` to store the aliases.
#### - They can be of the form ```alias command = real_command``` or ```command = real_command```. Note that there must be space before and after ```=```.      
### Redirection and Piping
#### - I referred to the working of bash in case of any conflict for how a given command should be executed.
### Activities  
#### - Zombie processes will be shown as "Zombie" status.
