# -*- coding: UTF-8 -*-

'''
作为开发环境首次初始化
'''
import platform

def ForWin():
  print('Init for windows')

  # 启动 mosquitto

  # 启动 mysql

  # 判断数据库是否已经初始化过了

  pass

def main():
  sys_info = platform.platform()
  if sys_info.find('Windows') != -1:
    ForWin()
  
  return

if __name__ == '__main__':
  main()
