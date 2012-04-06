/*
 * log.h
 *
 *  Created on: Apr 5, 2012
 *      Author: arash
 */

#ifndef LOG_H_
#define LOG_H_

#define LOG_FATAL	5
#define	LOG_ERROR	4
#define LOG_WARNING	3
#define LOG_INFO	2
#define LOG_DEBUG	1

#define LOG_LEVEL	LOG_DEBUG

#define FILE	strrchr(__FILE__,'/')?(strrchr(__FILE__,'/')+1):__FILE__

#if LOG_LEVEL <= LOG_ERROR
#define ERR(format, ...)	printk(KERN_ERR"[%s:%s():%d] "format, FILE, __func__, __LINE__,##__VA_ARGS__)
#else
#define ERR(format, ...)
#endif

#if LOG_LEVEL <= LOG_WARNING
#define WRN(format, ...)	printk(KERN_WARNING"[%s:%s():%d] "format, FILE, __func__, __LINE__,##__VA_ARGS__)
#else
#define WRN(format, ...)
#endif

#if LOG_LEVEL <= LOG_INFO
#define INF(format, ...)	printk(KERN_INFO"[%s:%s():%d] "format, FILE, __func__, __LINE__,##__VA_ARGS__)
#else
#define INF(format, ...)
#endif

#if LOG_LEVEL <= LOG_DEBUG
#define DBG(format, ...)	printk(KERN_DEBUG"[%s:%s():%d] "format, FILE, __func__, __LINE__,##__VA_ARGS__)
#else
#define DBG(format, ...)
#endif



#endif /* LOG_H_ */
