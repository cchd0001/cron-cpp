# cron-cpp 

## What is this ?  

This is a cron libriary write by c++ .

###Support Format : 

```
┌─────────────sec ( 0 - 59 ) 
| ┌───────────── min (0 - 59) 
| │ ┌────────────── hour (0 - 23)
| │ │ ┌─────────────── day of month (1 - 31)
| │ │ │ ┌──────────────── month (1 - 12)
| │ │ │ │ ┌───────────────── day of week (0 - 6) (0 to 6 are Sunday to Saturday, or use names; )
| │ │ │ │ │
| │ │ │ │ │
* * * * * *   
```

|Field name| 	Mandatory?| 	Allowed values |	Allowed special characters |
|:--------:|:------------:|:---------------:|:--------------------------|
|Minutes |	Yes |	0-59 |	* , - ? 	|
|Minutes |	Yes |	0-59 |	* , -  ?	|
|Hours |	Yes |	0-23 |	* , -  ?	|
| Day of month |	Yes | 	1-31 | 	* , - ? |
| Month |	Yes |	1-12 or JAN-DEC |	* , - ?	|
|Day of week| 	No | 	0-6 or SUN-SAT | 	* , - ? |

### Used 3rd libriary
1. boost 
2. UnitTest++  ( only if you want to compiler test code ) 

### Basic usage
#### Basic defines
*  Namespace :   ```Cron```  
*  Basic swap for boost library : 

```c++
typedef boost::asio::io_service IOService;
```
*  Job function 

```c++
typedef std::function<void()> Job;
```
*  Log level and log function : 

```c++
enum CronLogLevel {
  Unknow = 0 ,
  Info = 1,
  Debug = 2 ,
  Warning = 3 ,
  Error = 4 ,
  Fatel = 5
};

// Where Cron print it's logs
typedef std::function<void( CronLogLevel lev ,  const std::string & string)> CronLogger;
```
*  Cron manager class ```Cron``` .
#### Interfaces

*  Cron construct  :  ```Cron(IOService & s ,  CronLogger l) ```
  1.  ```Cron``` doesn't manager ```IOService``` .
  2.  ```Cron``` and all jobs that ```Cron``` manager are running on this ```IOService``` .

* Cron start  : ``` void Start()``` .
* Cron stop : ```void Stop()``` .
  1. ```Stop``` interface will top ```Cron``` . No more jobs will triggered .
  2. Jobs that already triggered will not be stoped .
* Add a job into cron ```int AddJob(const std::string & pattern , Job j);```
  1. ```AddJob``` is thread-safe inteface .
  2. you can call ```AddJob``` anytime .
  3. return ```int``` is a handler of this job, use it to remove this job . 
* Remove a specific job : ```void RemoveJob(int id)``` .
  1 ```RemoveJob``` is thread-safe inteface .
  2. you can call ```RemoveJob``` anytime .

### Example

## Why does I write this ?  

At first I hope to find a cron libriary for c++ language , but after searching from github  ， it seems no highly stared 
repository of c++ cron libriary . Instead I find https://github.com/robfig/cron ,a cron libriary of Go language which has hundrends of stars . 
So I decided to rewrite it by c++ .


## Referecnce 

###cron language 
https://en.wikipedia.org/wiki/Cron

### rofig/cron 
https://github.com/robfig/cron
