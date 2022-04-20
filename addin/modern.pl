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

-- MATCH(lookup_value, lookup_array, [match_type])
procedure Match (integer value, Range table, integer mtype) returns (integer result)
    signals (Err, NA)
    purpose ("Searches for a specified item in a range of cells") is

    integer count = 0

    result = 0

    -- This only works on one-dimensional tables.
    if table.Columns <> 1 & table.Rows <> 1 then
        signal Err
    end if

    if table.Sheets <> 1 then
        signal Err
    end if

    begin
        for integer i = 1 to table.Sheets loop
            for integer j = 1 to table.Columns loop
                for integer k = 1 to table.Rows loop
                    count = count + 1
                    case mtype
                        of 1:
                        -- MATCH finds the largest value that is less than or equal to lookup_value
                            if table[i, j, k] <= value then
                                result = count
                            end if
                        of 0:
                        -- MATCH finds the first value that is exactly equal to lookup_value
                            if table[i, j, k] = value then
                                result = count
                                return
                            end if
                        of -1:
                        -- MATCH finds the smallest value that is greater than or equal to lookup_value
                            if table[i, j, k] >= value then
                                result = count
                            end if
                    end case
                end loop
            end loop
        end loop

        -- If MATCH is unsuccessful in finding a match, it returns the #N/A error value.
        if result = 0 then
            signal NA
        end if
    end
    except on others (string sig) do
        Print("Signal ", sig, " raised.")
    end except

end procedure Match

