# cron-cpp 

## What is this ?  

This is a cron libriary write by c++ .

### current support 

Format : 

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


|:---:|:---:|:---:|:---:|
|Field| name| 	Mandatory?| 	Allowed values |	Allowed special characters |
|:---:|:---:|:---:|:---:|
|Minutes |	Yes |	0-59 |	* , - 	|

Hours 	Yes 	0-23 	* , - 	-
Day of month 	Yes 	1-31 	* , - ? L W 	-
Month 	Yes 	1-12 or JAN-DEC 	* , - 	-
Day of week 	Yes 	0-6 or SUN-SAT 	* , - ? L # 	-
Year 	No 	1970–2099 	* , - 	This field is not supported in standard/default implementations.

## Why does I write this ?  

At first I hope to find a cron libriary for c++ language , but after searching from github  ， it seems no highly stared 
repository of c++ cron libriary . Instead I find https://github.com/robfig/cron ,a cron libriary of Go language which has hundrends of stars . 
So I decided to rewrite it by c++ .


## Referecnce 

###cron language 
https://en.wikipedia.org/wiki/Cron
