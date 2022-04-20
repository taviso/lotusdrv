-- vim: set ft=rexx:
--
-- This is an attempt to implement some modern @FUNCTIONS in 1-2-3 DOS.
--
-- Tavis Ormandy <taviso@gmail.com>, April 2022
--

module modern is

use Register
use Range
use Cell
use AtStr
use Utility

Register("Match", At_Function)
    except on others do
        Print("Unable to register @MATCH")
    end except

-- MATCH(lookup_value, lookup_array, [match_type])
procedure Match (integer value, Range table, integer mtype) returns (integer result)
    signals (Err, NA)
    purpose ("Searches for a specified item in a range of cells") is
    integer cnt = 0

    begin
        for integer i = 1 to table.Sheets loop
            for integer j = 1 to table.Columns loop
                for integer k = 1 to table.Rows loop

                    cnt = cnt + 1

                    if table[i, j, k] = value then
                        result = cnt
                        return
                    end if

                    -- case rng.DataType (i, j, k)
                    --    of StringData:
                    -- end case
                    --    except on others do
                    --        signal Err
                    --    end except
                end loop
            end loop
        end loop

        -- If MATCH is unsuccessful in finding a match, it returns the #N/A error value.
        signal NA
    end
    except on others (string sig) do
        Print("Signal ", sig, " raised.")
    end except

end procedure Match

