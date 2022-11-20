#! /bin/python3

# Elias-Gamma numbers code generator.
# -----------------------------------
# This python script generates 256 Elias-Gamma coded numbers.
#
# Copyright (C) 2022 Abraham Macias Paredes.
#  
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Calculates an Elias-Gamma number
def generate_elias_gamma(n):
    # Calculate exponent
    e = 0
    i = n
    m = 0
    while i > 0:
        e = e + 1
        i = (i>>1)
        m = ((m<<1) | 1)
    
    e = e - 1
    m = (m>>1)
  
    r = (n & m)

    # Write exponent in unary
    eg_bin = ''
    for b in range(0, e):
        eg_bin = eg_bin + '0'
    eg_bin = eg_bin + '1'

    if e>0:
        # Write the remaining binary digits
        r = str(bin(r))[2:]
        while len(r) < e:
            r = '0' + r
        eg_bin = eg_bin + r

    # Store the Elias-Gamma number in hex and its bit length
    eg_hex = hex(int(eg_bin, 2))
    return (n, eg_hex, len(eg_bin))

# Generate 256 Elias-Gamma numbers
generated = []
for i in range(1, 257):
    generated.append(generate_elias_gamma(i))

# Print the 256 Elias-Gamma numbers in a file
f1=open('fast_eg.h', 'w+')
f1.write("int fastEliasGamma[256][2] = {\n")
for i in range(0, 256):
    f1.write("\t{ %s, %s }, // %s\n"%(generated[i][1], generated[i][2], generated[i][0]))
f1.write("};\n")
f1.close()
