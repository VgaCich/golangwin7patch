unit SHA1Hash;

interface

uses Classes;

function SHA1(const Data; Size: Integer): string;
function SHA1Stream(Stream: TCustomMemoryStream): string;
function SHA1File(const FileName: string): string;

implementation

function SHA1(const Data; Size: Integer): string;
const
  hlen = 20;
  iv: packed array[0..4] of Cardinal = ($67452301, $EFCDAB89, $98BADCFE, $10325476, $C3D2E1F0);
var
  Tail: packed array[0..127] of Byte;
  i, TailSize: Integer;
  TempSize: Int64;
  P: Pointer;
  f, k, t: Cardinal;
  h, s: packed array[0..4] of Cardinal;
  w: packed array[0..79] of Cardinal;

  procedure AddTail(B: Byte);
  begin
    Tail[TailSize] := B;
    Inc(TailSize);
  end;

  function GetBlock: Boolean;
  begin
    Result := Size + TailSize > 0;
    if not Result then Exit;
    if Size = 0 then
    begin
      P := @Tail;
      Size := TailSize;
      TailSize := 0;
    end;
    Move(P^, w, 64);
    Inc(Cardinal(P), 64);
    Dec(Size, 64);
  end;

  function Swap(D: Cardinal): Cardinal;
  asm
    BSWAP EAX
  end;

  function ROL(D: Cardinal; S: Integer): Cardinal;
  asm
    PUSH ECX
    MOV ECX, EDX
    ROL EAX, CL
    POP ECX
  end;

begin
  TailSize := Size mod 64;
  TempSize := 8 * Size;
  Size := Size - TailSize;
  P := @Data;
  Move(Pointer(Integer(P) + Size)^, Tail, TailSize);
  AddTail($80);
  while TailSize mod 64 <> 56 do
    AddTail(0);
  for i := 7 downto 0 do
    AddTail((TempSize shr (8 * i)) and $FF);
  Move(iv, h, hlen);
  while GetBlock do
  begin
    for i := 0 to 15 do
      w[i] := Swap(w[i]);
    for i := 16 to 79 do
      w[i] := ROL(w[i - 3] xor w[i - 8] xor w[i - 14] xor w[i - 16], 1);
    Move(h, s, hlen);
    for i := 0 to 79 do
    begin
      if i < 20 then
      begin
        f := (s[1] and s[2]) or ((not s[1]) and s[3]);
        k := $5A827999;
      end
      else if i < 40 then
      begin
        f := s[1] xor s[2] xor s[3];
        k := $6ED9EBA1;
      end
      else if i < 60 then
      begin
        f := (s[1] and s[2]) or (s[1] and s[3]) or (s[2] and s[3]);
        k := $8F1BBCDC;
      end
      else begin
        f := s[1] xor s[2] xor s[3];
        k := $CA62C1D6;
      end;
      t := ROL(s[0], 5) + f + s[4] + k + w[i];
      s[4] := s[3];
      s[3] := s[2];
      s[2] := ROL(s[1], 30);
      s[1] := s[0];
      s[0] := t;
    end;
    for i := 0 to 4 do
      h[i] := h[i] + s[i];
  end;
  for i := 0 to 4 do
    h[i] := Swap(h[i]);
  SetLength(Result, hlen);
  Move(h, Result[1], hlen);
end;

function SHA1Stream(Stream: TCustomMemoryStream): string;
begin
  Result := SHA1(Stream.Memory^, Stream.Size);
end;

function SHA1File(const FileName: string): string;
var
  Stream: TMemoryStream;
begin
  Stream := TMemoryStream.Create;
  try
    Stream.LoadFromFile(FileName);
    Result := SHA1Stream(Stream);
  finally
    Stream.Free;
  end;
end;

end.