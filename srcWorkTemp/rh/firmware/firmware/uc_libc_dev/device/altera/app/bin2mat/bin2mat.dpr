program bin2mat;

{$APPTYPE CONSOLE}

uses
  SysUtils, Classes;

(*
const char altera_bin[] = {
	"\x00\x01\x02\x03"
};
*)

var
	reverse: boolean = false;

procedure convert(src,dst: string);
var
  i,j : integer;
  fo,fd : TFileStream;
	s: ansistring;
  b : byte;
begin

  if not FileExists(src) then
  begin
    Writeln('file not found <'+src+'>');
    exit;
  end;

  Writeln('converting file <'+src+'>');

  fo := TFileStream.Create(src,fmOpenRead);
  fd := TFileStream.Create(dst,fmCreate);

  Writeln('filesize: '+IntToStr(fo.Size)+' bytes');

	s := 'const char altera_bin['+AnsiString(IntToStr(fo.Size))+'] = {';
  fd.Write(s[1],length(s));

  j := 0;
  fo.Seek(0,soFromBeginning);
  for i:=0 to fo.Size-1 do
	begin
		fo.Read(b,1);
		if reverse then b := (b * $0202020202 and $010884422010) mod 1023;
    if (j=0) then s := #13+#10+'"';
		s := s + '\x' + AnsiString(IntToHex(b,2));
    inc(j);
    if (j=16) then
    begin
      s := s + '"';
      fd.Write(s[1],length(s));
      j := 0;
    end;
  end;

  if j<>0 then
  begin
    s := s + '"';
    fd.Write(s[1],length(s));
  end;
  s := #13+#10+'};'+#13#10;
  fd.Write(s[1],length(s));

  fo.Free;
  fd.Free;

end;

var
  filename : string;
	i: integer;

begin
	try
		{ TODO -oUser -cConsole Main : Insert code here }
		for i:=0 to ParamCount do
		begin
			if (ParamStr(i)='-bin') then filename := ParamStr(i+1);
			if (ParamStr(i)='-r') then reverse := true;
		end;
    convert(filename,ChangeFileExt(filename,'.h'));
  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
