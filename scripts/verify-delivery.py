import hashlib,json,pathlib,struct,zipfile,sys
root=pathlib.Path(sys.argv[1] if len(sys.argv)>1 else 'out/delivery-0.2')
status=json.loads((root/'status.json').read_text())
assert status['phase']=='success'
expected={}
for line in (root/'evidence-SHA256SUMS.txt').read_text(encoding='utf-8-sig').splitlines():
 if line.strip():
  digest,name=line.split(None,1);expected[name.strip()]=digest.lower()
verified={}
for name,digest in expected.items():
 file=root/name
 assert file.is_file(),name
 actual=hashlib.sha256(file.read_bytes()).hexdigest()
 assert actual==digest, name+' checksum mismatch'
 verified[name]={'bytes':file.stat().st_size,'sha256':actual}
name='JerzyVoltageGarden-0.2.0-Windows-x64.zip'
bundle='Jerzy Voltage Garden.vst3'
member=bundle+'/Contents/x86_64-win/Jerzy Voltage Garden.vst3'
with zipfile.ZipFile(root/name) as archive:
 assert archive.testzip() is None
 names={p.replace('\\','/') for p in archive.namelist()}
 assert member in names
 for info in archive.infolist():
  relative=pathlib.PurePosixPath(info.filename.replace('\\','/'))
  assert not relative.is_absolute() and '..' not in relative.parts
 binary=archive.read(member)
 assert binary[:2]==b'MZ'
 offset=struct.unpack_from('<I',binary,0x3c)[0]
 assert binary[offset:offset+4]==b'PE\x00\x00'
 assert struct.unpack_from('<H',binary,offset+4)[0]==0x8664
 assert struct.unpack_from('<H',binary,offset+24)[0]==0x20b
 assert struct.unpack_from('<H',binary,offset+22)[0]&0x2000
 sections=struct.unpack_from('<H',binary,offset+6)[0]
 optional_size=struct.unpack_from('<H',binary,offset+20)[0]
 section_table=offset+24+optional_size
 def file_offset(rva):
  for index in range(sections):
   entry=section_table+40*index
   virtual_size,virtual_address,raw_size,raw_address=struct.unpack_from('<IIII',binary,entry+8)
   if virtual_address<=rva<virtual_address+max(virtual_size,raw_size):return raw_address+rva-virtual_address
  raise AssertionError('Unmapped PE RVA')
 export_rva=struct.unpack_from('<I',binary,offset+24+112)[0]
 export_directory=file_offset(export_rva)
 names_count=struct.unpack_from('<I',binary,export_directory+24)[0]
 names_table=file_offset(struct.unpack_from('<I',binary,export_directory+32)[0])
 exports=[]
 for index in range(names_count):
  name_offset=file_offset(struct.unpack_from('<I',binary,names_table+4*index)[0])
  exports.append(binary[name_offset:binary.index(b'\x00',name_offset)].decode('ascii'))
 assert 'GetPluginFactory' in exports
 identity=archive.read('BUILD.txt').decode('utf-8-sig')
 assert status['head_sha'] in identity
 for line in archive.read('VST3-SHA256SUMS.txt').decode('utf-8-sig').splitlines():
  if line.strip():
   digest,path=line.split(None,1)
   contents=archive.read(bundle+'/'+path.strip().replace('\\','/'))
   assert hashlib.sha256(contents).hexdigest()==digest.lower()
 for info in archive.infolist():
  if info.filename.replace('\\','/').startswith(bundle+'/'):
   target=root/pathlib.Path(info.filename.replace('\\','/'))
   if info.is_dir():target.mkdir(parents=True,exist_ok=True)
   else:target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(archive.read(info))
with zipfile.ZipFile(root/'JerzyVoltageGarden-0.2.0-Source.zip') as source:
 assert source.testzip() is None
 assert not any('/.git/' in '/'+name or name.rstrip('/').endswith('/.git') for name in source.namelist()), 'Git metadata leaked into source archive'
 assert (root/'JerzyVoltageGarden-0.2.0-Source.zip').stat().st_size<80_000_000
 assert b'project(JerzyVoltageGarden' in source.read('CMakeLists.txt')
 for name in ('src/Engine.h','src/Editor.cpp','JUCE/LICENSE.md','LICENSES/AGPL-3.0.txt'):
  assert source.getinfo(name).file_size>0
report={'source_commit':status['head_sha'],'run_url':status['run_url'],'pe_machine':'AMD64 / PE32+ / DLL','exports':exports,'bundle_binary_sha256':hashlib.sha256(binary).hexdigest(),'verified_files':verified,'windows_checks':status['steps']}
(root/'Jerzy-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
