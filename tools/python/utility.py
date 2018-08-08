'''
  各种用到的工具集
'''

import os
import codecs

__util_last_error = ''

# 检测路径
#   path  需要检测的路径
#   is_mk_dir True,如果路径不存在，则会创建
#             False,如果路径不存在，直接返回False
#   return True\False
def CheckPath(path, is_mk_dir = True):
  if len(path) == 0:
    SetLastError('path 为空')
    return False
  
  is_dir = os.path.isdir(path)
  
  if is_mk_dir == False:
    if is_dir == True:
      return True
    else:
      SetLastError('路径不存在')
      return False
  
  path = os.path.abspath(path)
  last_pos = 0

  # 错误的盘符 Windows 下
  i = path.find(':')
  if i == -1:
    SetLastError('未找到盘符')
    return False
  cur_path = path[ : i + 1]
  if os.path.isdir(cur_path) == False:
    SetLastError('盘符不存在')
    return False
  last_pos = i + 2
  
  # 遍历目录，检测创建
  while (True):
    i = path.find('\\', last_pos)
    if i == -1:
      cur_path = path
    else:
      cur_path = path[ : i]
    if os.path.isdir(cur_path) == False:
      try:
        os.mkdir(cur_path)
      except OSError as error:
        pass
    if i == -1:
      break
    last_pos = i + 1

  return True

# 从文件中读取数据，并已行列表的形式输出
def ReadFileToLines(path):
  if os.path.isfile(path) == False:
    SetLastError('读取文件--文件路径不存在')
    return None
  
  try:
    f = codecs.open(path, mode='r', encoding='utf8')
    d = f.readlines()
    f.close()
  except OSError as error:
    SetLastError('读取文件错误:' + __OutOSErrorInfo(error))
    return None
  
  # 如果UTF8带签名，去掉签名
  if len(d) > 0 and d[0][ : 1] == '\ufeff':
    d[0] = d[0][1: ]
  return d

def ReadFileToStr(path):
  if os.path.isfile(path) == False:
    SetLastError('读取文件失败--文件不存在')
    return None
  try:
    f = codecs.open(path, mode='r', encoding='utf8')
    ref_str = f.read()
    f.close()
  except OSError as error:
    SetLastError('读取文件失败--' + __OutOSErrorInfo(error))
    return None
  if len(ref_str) > 0 and ref_str[:1] == '\ufeff':
    ref_str = ref_str[1: ]
  return ref_str

def WriteFileToStr(path, data, with_bom=False):
  '''if os.path.isfile(path) == False:
    SetLastError('写入文件失败--文件不存在')
    return False'''
  try:
    if with_bom == True:
      data = '\ufeff' + data
    f = codecs.open(path, mode='w', encoding='utf8')
    f.write(data)
    f.close()
  except OSError as error:
    SetLastError('写入文件失败--' + __OutOSErrorInfo(error))
    return False
  return True

# 删除文本开头的各种字符
def DelBeginChar(d):
  i = 0
  for c in d:
    if c != ' ' and c != '\r' and c != '\n':
      break
    else:
      i += 1
  return d[i :]

# 删除文本结尾的各种字符
def DelEndChar(d):
  rd = d[:: -1]
  i = 0
  for c in rd:
    if c != ' ' and c != '\r' and c != '\n':
      break
    else:
      i += 1
  i = len(d) - i
  return d[: i]

# 判断模板信息中的注释
def IsComments(line):
  for c in line:
    if c == '#':
      return True
    elif c == ' ' or c == '\r' or c == '\n':
      continue
    return False

# 替换模板中的宏为目标文本
def RepTempStr(temp_macro, d):
  for macro in temp_macro:
    if len(macro) != 2:
      continue
    old = '$(--' + macro[0] + '--)'
    new = macro[1]
    d = d.replace(old, new)
  return d

def IsCCode(path):
  ext = os.path.splitext(path)[1]
  ext = ext.lower()

  c_ext_path = [
    '.h',
    '.hpp',
    '.c',
    '.cc',
    '.cpp']
  
  for def_ext in c_ext_path:
    if ext == def_ext:
      return True

  return False

# 替换模板中的代码
def RepTempCode(temp_macro, path):
  str_data = ReadFileToStr(path)
  if str_data == None:
    SetLastError('复制模板--读取文件错误:' + GetLastError())
    return False
  str_data = RepTempStr(temp_macro, str_data)

  with_bom = IsCCode(path)
  if WriteFileToStr(path, str_data, with_bom) == False:
    SetLastError('复制模板--写入文件错误:' + GetLastError())
    return False
  return True

def SetLastError(err):
  global __util_last_error
  __util_last_error = err

def GetLastError():
  global __util_last_error
  return __util_last_error

# 输出 OSError 异常文本
def __OutOSErrorInfo(error):
  ref_str = ''

  ref_str += '||errno:' + error.errno
  ref_str += '||winerror:' + error.winerror
  ref_str += '||strerror:' + error.strerror

  ref_str += '||path1:' + error.filename
  ref_str += '||path2:' + error.filename2

  return ref_str

##################################################
# 测试
#   测试会分不同情况进行测试...

# 打印开始和结束
class PrintInfo:
  def __init__(self, name):
    self.__name = name
    self.__delimiter = '--------'
    print(self.__delimiter + 'BEGIN--' + self.__name + self.__delimiter)
  
  def __del__(self):
    print(self.__delimiter + 'END--' + self.__name + self.__delimiter)
    
def __testCheckPath():
  info = PrintInfo('CheckPath')

  # 传入空路径
  b_ref = CheckPath('')
  err = GetLastError()
  if b_ref == True or len(err) == 0:
    print('空路径测试失败，ReturnVal:' +str(b_ref) + '，Error:' + err)
    return

  # 检测一个存在的路径
  b_ref = CheckPath('C:\\Windows', False)
  if b_ref == False:
    print('检测一个存在的路径返回错误, error:' + GetLastError())
    return
  
  # 检测一个不存在的路径
  b_ref = CheckPath('S:\\12346', False)
  if b_ref == True or len(err) == 0:
    print('检测一个不存在的路径返回错误, ReturnVal:' +str(b_ref) + '，Error:' + err)
    return
  
  # 在不存在的路径创建：
  test_path = 'D:\\tmp\\123\\456\\abc'
  b_ref = CheckPath(test_path)
  if b_ref == False:
    print('创建一个已经存在的路径错误，Error:' + GetLastError())
    return
  if os.path.isdir(test_path) == False:
    print('创建目录失败')
    return

def __testReadFile():
  info = PrintInfo('ReadFile')

  file_path = 'D:\\tmp\\test_data.txt'
  b_ref = ReadFileToLines(file_path)
  if b_ref == None:
    print('读取文件失败--Error:' + GetLastError())
    return

def __test():
  print('----------------测试开始----------------')

  #__testCheckPath()
  __testReadFile()

  print('----------------测试结束----------------')

if __name__ == '__main__':
  __test()

# end
##################################################
