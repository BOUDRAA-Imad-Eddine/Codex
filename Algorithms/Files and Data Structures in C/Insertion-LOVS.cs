Procedure Inser_LOVS(filename, key : char[20], Rec : char[])
Begin
    
    searchLOVS(filename,key); 

    If (found = True) then
        write("record existant");
    else
        open(F, filename, "E");
        size_to_insert ← length(data) + 24;
        setHeader(F, 3, (getHeader(F,3) + size_to_insert) mod b);
        record = length(data) + "N" + key + data;
        stop = False;

       
        while (Not stop) do
            readBlock(F, i, Buf);

            If (size_to_insert <= b) Then
                For k = 1 to size_to_insert do
                    tmp[k] = Buf.tab[j + k];
                    Buf.tab[j + k - 1] = rec[k];
                EndFor;
                writeBlock(F, i, Buf);
                j= j + size_to_insert;
                stop = True;


            Else
                x = j + size_to_insert - b;
                k = 1;

                while (j <= b) do
                    tmp[k] ← Buf.tab[j];
                    Buf.tab[j] ← record[k];
                    j = j + 1;
                    k = k + 1;
                EndWhile;

                writeBlock(F, i, Buf);

      
                i =Buf.next;
                j=1;

                
                while (x > 0) do
                   
                    If (i = -1) then
                        allocBlock(F);
                        setHeader(F, 2, getHeader(F, 2) + 1);
                        i = getHeader(F, 2);
                    EndIf;

                    readBlock(F, i, Buf);

                   
                    If (x > b) then
                        y = b;
                    else
                        y = x;
                    EndIf;

                   
                    For 1 to y do
                        Buf.tab[j] ← record[k];
                        j ← j + 1;
                        k ← k + 1;
                    EndFor;

                    writeBlock(F, i, Buf);

                    
                    x=  x - y;

                   
                    If (x > 0) then
                        i =Buf.next;
                        j =1;
                    EndIf;
                EndWhile;

                stop =True;
            EndIf;

          
            If (i = getHeader(F,2) and j = getHeader(F,3)) then
                stop =True;
            EndIf;
        EndWhile;

        close(file);
    EndIf;
End;
