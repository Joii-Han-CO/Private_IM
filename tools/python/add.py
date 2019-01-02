# coding=utf-8

import os
import sys
import shutil
import utility
import git
import argparse

class AddProject:
  def __init__(self, prj_name, prj_folder, sln_path = ''):
    self.__last_error = ''

    self.__prj_name = prj_name
    self.__prj_folder = prj_folder
    self.__prj_src_path = ''
    self.__sln_path = sln_path
    self.__sln_name = ''
    self.__sln_namespace = ''
    
    self.__temp_path = ''
    self.__temp_macro = []  # 模板中需要替换的宏列表
    self.__temp_files = []  # 模板复制的文件

    pass
  
  ##################################################
  # 初始化

  # 获取sln的路径
  def __GetSlnPath(self):
    if len(self.__sln_path) > 0:
      if os.path.isdir(self.__sln_path) == True:
        return True
      else:
        self.__last_error = '初始化--输入的工程路径不存在'
    else:
      file_path = os.path.realpath(__file__)
      self.__sln_path = os.path.dirname(file_path) + '\\..\\'
    self.__sln_path = os.path.abspath(self.__sln_path)
    return True

  # 检测路径
  def __GetSlnInfo(self):
    info_path = self.__sln_path + '\\tools\\project_info.txt'
    lines = utility.ReadFileToLines(info_path)
    if lines == None:
      self.__last_error = '读取工程信息错误--' + utility.GetLastError()
      return False
    sln_map = {}
    for line in lines:
      line = utility.DelBeginChar(line)
      line = utility.DelEndChar(line)
      if len(line) == 0:
        continue
      i = line.find(':')
      if i == -1:
        continue
      key = line[: i]
      val = line[i + 1 :]
      sln_map[key] = val
    self.__sln_name = sln_map['sln_name']
    self.__sln_namespace = sln_map['namespace']

    print('获取到 name:' + self.__sln_name)
    print('获取到 namespace:' + self.__sln_namespace)

    return True

  # 初始化各种数据的信息
  def __InitInfo(self):

    self.__prj_src_path = self.__sln_path + '\\src\\' + \
      self.__prj_folder + '\\' + self.__prj_name

    # 模板路径
    self.__temp_path = self.__sln_path + '\\tools\\project_temp\\'
    self.__temp_path = os.path.abspath(self.__temp_path)

    # 宏
    self.__temp_macro = []
    self.__temp_macro.append(['namespace', self.__sln_namespace])
    self.__temp_macro.append(['sln_name', self.__sln_name])
    self.__temp_macro.append(['prj_name', self.__prj_name])
    self.__temp_macro.append(['prj_folder', self.__prj_folder])
    self.__temp_macro.append(['NAMESPACE', self.__sln_namespace.upper()])
    self.__temp_macro.append(['SLN_NAME', self.__sln_name.upper()])
    self.__temp_macro.append(['PRJ_FOLDER', self.__prj_folder.upper()])

    return True

  def __CheckPath(self):
    if utility.CheckPath(self.__temp_path, False) == False:
      self.__last_error = '未检测到模板目录--' + utility.GetLastError()
      return False
    
    if utility.CheckPath(self.__prj_src_path) == False:
      self.__last_error = '检测源码路径错误--' + utility.GetLastError()
      return False
    
    if utility.CheckPath(self.__prj_src_path + '\\test') == False:
      self.__last_error = '检测测试路径错误--' + utility.GetLastError()
      return False

    return True

  # end
  ##################################################

  ##################################################
  # 模板相关

  def __GetTempFilesList(self):
    temp_info = self.__temp_path + '\\temp_list.txt'
    lines = utility.ReadFileToLines(temp_info)
    if lines == None:
      self.__last_error = '获取模板信息错误--' + utility.GetLastError()
      return False
    for line in lines:
      line = utility.DelBeginChar(line)
      line = utility.DelEndChar(line)
      if len(line) == 0:
        continue
      i = line.find('-->')
      if i == -1:
        continue
      key = line[: i]
      val = line[i + 3 : ]
      self.__temp_files.append([key, val])
    return True

  def __CopyTempFiles(self):
    for fm in self.__temp_files:
      if len(fm) != 2:
        continue
      if len(fm[0]) == 0 or len(fm[1]) == 0:
        continue
      src_path = self.__temp_path + '\\' + fm[0]
      dst_path = self.__sln_path + '\\' + utility.RepTempStr(self.__temp_macro, fm[1])
      shutil.copy(src_path, dst_path)
      utility.RepTempCode(self.__temp_macro, dst_path)
    return True

  # end
  ##################################################
  
  ##################################################
  # 对外接口

  # 初始化
  def Init(self):
    print('初始化')

    if self.__GetSlnPath() == False:
      return False
    print('sln path:' + self.__sln_path)
    
    if self.__GetSlnInfo() == False:
      return False

    if self.__InitInfo() == False:
      return False
    
    if self.__CheckPath() == False:
      return False

    return True

  # 复制模板
  def CopyTemp(self):
    print('复制模板')

    if self.__GetTempFilesList() == False:
      return False
    
    if self.__CopyTempFiles() == False:
      return False
    return True
  
  def AddCMake(self):
    cmake_path = self.__sln_path + '\\src\\CMakeLists.txt'
    data = utility.ReadFileToStr(cmake_path)
    if data == None:
      self.__last_error = '读取CMake主工程文件错误' + utility.GetLastError()
      return False

    data = utility.DelEndChar(data)

    data += '\r\n\r\n# ' + self.__prj_name + '\r\n'
    data += 'add_subdirectory(${PROJECT_SOURCE_DIR}/' + \
      self.__prj_folder + '/' + self.__prj_name + '/)'

    if utility.WriteFileToStr(cmake_path, data) == False:
      self.__last_error = '写入CMake主工程文件错误' + utility.GetLastError()
      return False

    return True

  def GetLastError(self):
    return self.__last_error

  def UsingGit(self):
    try:
      repo = git.Repo(self.__sln_path)
    except git.InvalidGitRepositoryError as er:
      self.__last_error = '无法打开Git项目'
      return False
    repo.index.add(['project', 'src'])
    repo.index.commit('add : ' + self.__prj_name)
    repo.__del__()
    return True

  # end
  ##################################################

def ParseArgs():
  parse = argparse.ArgumentParser()
  parse.add_argument('--path', '-p')
  parse.add_argument('--name', '-n')
  parse.add_argument('--folder', '-folder')

  args = parse.parse_args()
  return args

def main():

  # 需要解析参数
  args = ParseArgs()
  
  prj_name = 'fde_test'
  prj_folder = 'lib'
  #sln_path = r'D:\tmp\tpjr'
  sln_path = ''

  if args.path != None:
    sln_path = args.path
  if args.name != None:
    prj_name = args.name
  if args.folder != None:
    prj_folder = args.folder
  

  print('开始添加项目:' + prj_name)

  ap = AddProject(prj_name, prj_folder, sln_path)
  if ap.Init() == False:
    print('初始化失败--' + ap.GetLastError())
    return
  if ap.CopyTemp() == False:
    print('复制模板失败--' + ap.GetLastError())
    return
  
  if ap.AddCMake() == False:
    print('添加项目错误--' + ap.GetLastError())
    return
  
  '''if ap.UsingGit() == False:
    print('提交Git失败--' + ap.GetLastError())
    return'''

  print('项目添加完成')

if __name__ == '__main__':
  main()
