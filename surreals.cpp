///
/// Implementations for functions of Surreal and SurrealInf classes.
///

#include "surreals.h"

namespace surreals {

    /// Constructor from two sets of surreal numbers
    ///
    /// \param leftIn: the reference to the left set
    /// \param rightIn: the reference to the right set
    /// \param simplify: if this flag is set, the constructor only considers
    ///                  the greatest element from L and the smallest element from R.
    ///                  Numbers constructed with and without this flag are always equal.
    Surreal::Surreal(std::set<Surreal> const &leftIn,
                     std::set<Surreal> const &rightIn,
                     bool simplify = true) {

        /// Check that no element in the right set is less than or equal to any element in the left set.
        /// This prevents construction of pseudo-numbers.
        for (Surreal const &rightElem : rightIn) {
            for (Surreal const &leftElem : leftIn) {
                if (rightElem <= leftElem) {
                    throw std::runtime_error("Bad input sets during surreal number creation!");
                }
            }
        }

        if (simplify) {
            /// The simplify flag is set, so we consider only the greatest element from L and
            /// the smallest element from R. Arithmetic-wise, the resulting number will be the same.
            if (!leftIn.empty()) { left.emplace(*leftIn.rbegin()); }
            if (!rightIn.empty()) { right.emplace(*rightIn.begin()); }
        } else {
            /// The simplify flag is not set, so we consider every element from L and R.
            for (Surreal const &elem : leftIn) { left.emplace(elem); }
            for (Surreal const &elem : rightIn) { right.emplace(elem); }
        }
    }

    /// Constructor from an integer
    ///
    /// \param input: the input integer
    Surreal::Surreal(int const &input) {
        /// When a Surreal number is equivalent to a positive integer N, it has the form
        /// {{ ... {{{ {|} |} |} |} |} |} |} ... |}
        /// It is created on day N and effectively only contains a single zero at depth N,
        /// going left all the way. For a negative integer, the zero is on the right.
        ///
        /// The constructor starts with zero and nests it N times.

        Surreal res; /// the zero
        if (input == 0) { return; } /// if the input integer is 0, we're done.

        if (input > 0) {
            /// for a positive integer, nest the result on the left that many times.
            for (int i = 0; i < input; i++) {
                res = Surreal(std::set<Surreal>({res}), std::set<Surreal>());
            }
        } else {
            /// for a negative integer, nest on the right.
            for (int i = 0; i > input; i--) {
                res = Surreal(std::set<Surreal>(), std::set<Surreal>({res}));
            }
        }

        this->left = res.left;
        this->right = res.right;
    }

    /// Constructor from a float
    ///
    /// \param input: the input floating point number
    Surreal::Surreal(float const &input) {
        /// get floor and ceiling of the input float
        float float_floor = std::floor(input), float_ceil = std::ceil(input);

        /// if the input float is equal to an integer, use the integer constructor instead
        if (float_floor == float_ceil) {
            Surreal res = Surreal((int) float_floor);
            this->left = res.left;
            this->right = res.right;
            return;
        }

        /// The construction is iterative. The algorithm starts with 3 floating point numbers:
        /// the floor, the ceiling, and the middle (which is halfway between the floor and the ceiling).
        /// Those floats each have a corresponding Surreal number (sur_floor, sur_ceil, and sur_mid).
        /// Until float_mid is equal to the input float, the constructor repeats the following:
        ///
        /// If float_mid is greater than the input float, it becomes the new ceiling, and a new mid is created
        /// between it and the floor. If it is less than the input float, it becomes the new floor, and
        /// a new mid is created between it and the ceiling. The corresponding Surreals are adjusted accordingly.
        ///
        /// On each iteration, the depth of sur_mid is increased by one. The iteration ends when the float precision
        /// is insufficient to tell float_mid and the input apart.

        float float_mid = (float_floor + float_ceil) / 2;
        Surreal sur_floor = Surreal((int) float_floor); /// corresponds to float_floor
        Surreal sur_ceil = Surreal((int) float_ceil); /// corresponds to float_ceil
        Surreal sur_mid = Surreal(std::set<Surreal>({sur_floor}),
                                  std::set<Surreal>({sur_ceil})); /// corresponds to float_mid

        while (float_mid != input) { /// end the loop when the midpoint equals the input float
            if (input < float_mid) {
                /// adjust the floats
                float_ceil = float_mid;
                float_mid = (float_floor + float_ceil) / 2;

                /// make sur_ceil equal to sur_mid
                sur_ceil.left = sur_mid.left;
                sur_ceil.right = sur_mid.right;

                /// construct a new sur_mid using the old mid and the floor
                sur_mid.right = std::set<Surreal>({sur_mid});
                sur_mid.left = std::set<Surreal>({sur_floor});
            } else {
                /// adjust the floats
                float_floor = float_mid;
                float_mid = (float_floor + float_ceil) / 2;

                /// make sur_floor equal to sur_mid
                sur_floor.left = sur_mid.left;
                sur_floor.right = sur_mid.right;

                /// construct a new sur_mid using the old mid and the ceiling
                sur_mid.left = std::set<Surreal>({sur_mid});
                sur_mid.right = std::set<Surreal>({sur_ceil});
            }
        }

        this->left = sur_mid.left;
        this->right = sur_mid.right;
    }

    /// Copy constructor
    ///
    /// \param other: the Surreal to be copied
    Surreal::Surreal(Surreal const &other) {
        this->left = other.left;
        this->right = other.right;
    }

    /// Constructor from two references to Surreals
    ///
    /// \param sur_left: the left side
    /// \param sur_right: the right side
    Surreal::Surreal(const surreals::Surreal &sur_left, const surreals::Surreal &sur_right) {
        if (sur_left < sur_right) { /// check that we're not constructing a pseudo-number
            this->left.emplace(sur_left);
            this->right.emplace(sur_right);
        } else {
            throw std::runtime_error("Bad input numbers during surreal number creation!");
        }
    }

    /// Constructor from a SurrealInf. This construction will only be performed if the SurrealInf has
    /// finite left and right sets. It is recursive, so this restriction applies to every "child" of
    /// the input SurrealInf. If somewhere in the tree there exists an infinite set, an exception will be thrown.
    Surreal::Surreal(SurrealInf &inputSurInf) {

        int leftSize = inputSurInf.leftSize;
        int rightSize = inputSurInf.rightSize;

        /// throw exception if sets are infinite
        if (leftSize < 0 || rightSize < 0) {
            throw std::runtime_error("Encountered infinite set in Surreal::Surreal( SurrealInf const &inputSurInf )");
        }

        /// if sets are finite, recursively get the greatest number from L and smallest number from R
        std::set<Surreal> tempLset, tempRset;

        if (leftSize > 0) {
            try {
                SurrealInf tempLinf = inputSurInf.getLeft(leftSize - 1);
                Surreal tempLfin = Surreal(tempLinf);
                tempLset.insert(tempLfin);
            } catch (const std::runtime_error &e) {
                /// infinite sets in the child, rethrow error
                throw;
            }
        }

        if (rightSize > 0) {
            try {
                SurrealInf tempRinf = inputSurInf.getRight(rightSize - 1);
                Surreal tempRfin = Surreal(tempRinf);
                tempRset.insert(tempRfin);
            } catch (const std::runtime_error &e) {
                /// infinite sets in the child, rethrow error
                throw;
            }
        }

        Surreal res = Surreal(tempLset, tempRset, true);
        this->left = res.left;
        this->right = res.right;
    }

    /// Default constructor
    Surreal::Surreal() = default;

    /// Destructor
    Surreal::~Surreal() = default;

    /// Depth function
    ///
    /// \return the recursive depth of the Surreal number
    std::size_t Surreal::Depth() const {

        /// if L and R are both empty, depth is 0
        if (left.empty() && right.empty()) {
            return 0;
        }

        /// otherwise, return 1 + the max depth among numbers in L and R
        std::size_t leftMax = 0, rightMax = 0;
        for (Surreal const &leftElem : left) {
            std::size_t lDepth = leftElem.Depth();
            /// new left max depth?
            leftMax = (leftMax > lDepth) ? leftMax : lDepth;
        }
        for (Surreal const &rightElem : right) {
            std::size_t rDepth = rightElem.Depth();
            /// new right max depth?
            rightMax = (rightMax > rDepth) ? rightMax : rDepth;
        }

        return (leftMax >= rightMax) ? leftMax + 1 : rightMax + 1;
    }

    /// In-place Arithmetic

    /// In-place Addition
    ///
    /// \param other: the Surreal to add
    /// \return self by reference
    Surreal &Surreal::operator+=(Surreal const &other) {
        /// implemented using the binary operator
        Surreal tempSum = *this + other;
        this->left = tempSum.left;
        this->right = tempSum.right;
        return *this;
    }

    /// In-place Subtraction
    ///
    /// \param other: the Surreal to subtract
    /// \return self by reference
    Surreal &Surreal::operator-=(Surreal const &other) {
        /// implemented using the binary operator
        Surreal tempSub = *this - other;
        this->left = tempSub.left;
        this->right = tempSub.right;
        return *this;
    }

    /// In-place Multiplication
    ///
    /// \param other: the Surreal by which to multiply
    /// \return self by reference
    Surreal &Surreal::operator*=(Surreal const &other) {
        /// implemented using the binary operator
        Surreal tempSub = *this * other;
        this->left = tempSub.left;
        this->right = tempSub.right;
        return *this;
    }

    /// Arithmetic

    /// The addition lookup table
    std::map<std::pair<Surreal, Surreal>, Surreal> Surreal::AddLookup = std::map<std::pair<Surreal, Surreal>, Surreal>();

    /// Addition (Binary operator)
    ///
    /// \param a: an operand
    /// \param b: an operand
    /// \return the result of addition
    Surreal operator+(Surreal const &a, Surreal const &b) {
        /// Using a lookup table. If the requested pair of numbers is already there,
        /// skip the calculation and use the lookup value instead. Otherwise, proceed
        /// with the calculation and put the result into the lookup table for later use.

        /// Try finding the pair in the lookup table
        auto lookupIter = Surreal::AddLookup.find(std::pair<Surreal, Surreal>(std::minmax(a, b)));
        if (lookupIter != Surreal::AddLookup.end()) {
            return lookupIter->second;
        } /// the requested pair of operands is not found in the lookup table, proceed with calculation

        /// Addition on Surreals is defined as
        /// a + b = { Al + b, Bl + a | Ar + b, Br + a }
        /// the algorithm recursively constructs sets tempL1, tempL2, tempR1, tempR2 so that:
        /// a + b = { tempL1, tempL2 | tempR1, tempR2 }
        /// then constructs a Surreal number using those sets.

        std::set<Surreal> tempL1 = a.left + b;
        std::set<Surreal> tempR1 = a.right + b;
        std::set<Surreal> tempL2 = b.left + a;
        std::set<Surreal> tempR2 = b.right + a;

        tempL1.insert(tempL2.begin(), tempL2.end());
        tempR1.insert(tempR2.begin(), tempR2.end());

        Surreal res = Surreal(tempL1, tempR1);

        /// We check that the result does not have a simpler, equivalent representation in the addition lookup table.
        /// If it does, then use the simpler version instead. This prevents numbers with several terms in their sets from forming.
        ///
        /// "Simpler" means "less terms on the left and right side", for example { -1 | 1 } will be simplified to 0.

        /// store the number of terms in the result to reference it during simplification
        std::size_t resSize = res.left.size() + res.right.size();

        auto simplIter = Surreal::AddLookup.begin();
        while (simplIter != Surreal::AddLookup.end()) { /// step through the addition lookup table

            if (res == simplIter->second) { /// found an equivalent value, check if it is "simpler"

                /// size of the found equivalent
                std::size_t eqSize = simplIter->second.left.size() + simplIter->second.right.size();

                if (resSize > eqSize) {
                    /// result is more complex than an equivalent value in the lookup table.
                    /// replace the result with the lookup value and exit the search
                    res = simplIter->second;
                    break;
                } else if (resSize < eqSize) {
                    /// result is less complex than an equivalent value in the table.
                    /// replace the lookup value with the result and continue seaching.
                    ///
                    /// This should not happen in normal use due to how the recursion happens,
                    /// however it is possible to trigger this by manually creating numbers.
                    simplIter->second = res;
                } else {
                    /// result is equivalent and has the same complexity, use lookup value instead
                    /// and exit the search.
                    ///
                    /// In practice, as soon as we reach a value that is equivalent and also
                    /// has the same complexity, we can assume that other equivalent values in the table
                    /// have the same representation.
                    res = simplIter->second;
                    break;
                }
            }
            simplIter++;
        }

        /// insert the result into the lookup table for later use
        Surreal::AddLookup.emplace(std::pair<Surreal, Surreal>(std::minmax(a, b)), res);
        return res;
    }

    /// Negation
    ///
    /// \return the negated number
    Surreal Surreal::operator-() const {
        /// Recursively negate left and right sets
        std::set<Surreal> tempL = NegateSet(right);
        std::set<Surreal> tempR = NegateSet(left);
        return Surreal(tempL, tempR);
    }

    /// Subtraction (Binary operator)
    ///
    /// \param a: the operand
    /// \param b: the operand
    /// \return the result of subtraction
    Surreal operator-(Surreal const &a, Surreal const &b) {
        /// Implemented using addition and negation
        return a + (-b);
    }

    /// The multiplication lookup table
    std::map<std::pair<Surreal, Surreal>, Surreal>
            Surreal::MultLookup = std::map<std::pair<Surreal, Surreal>, Surreal>();

    /// multiplication
    Surreal operator*(Surreal const &a, Surreal const &b) {
        /// Using a lookup table. If the requested pair of numbers is already there,
        /// skip the calculation and use the lookup value instead. Otherwise, proceed
        /// with the calculation and put the result into the lookup table for later use.

        /// Try finding the pair in the lookup table
        auto lookupIter = Surreal::MultLookup.find(std::pair<Surreal, Surreal>(std::minmax(a, b)));
        if (lookupIter != Surreal::MultLookup.end()) {
            return lookupIter->second;
        } /// the requested pair of operands is not found in the lookup table, proceed with calculation

        /// Multiplication on Surreals is defined as
        /// a*b = { Al*b + a*Bl - Al*Bl, Ar*b + a*Br - Ar*Br | Al*b + a*Br - Al*Br, Ar*b + a*Bl - Ar*Bl }
        /// the algorithm recursively constructs sets tempL1, tempL2, tempR1, tempR2 so that:
        /// a*b = { tempL1, tempL2, | tempR1, tempR2 }
        /// then constructs a Surreal number using those sets.

        std::set<Surreal> Al_b = a.left * b; /// Al * b
        std::set<Surreal> Ar_b = a.right * b; /// Ar * b

        std::set<Surreal> Bl_a = b.left * a; /// Bl * a
        std::set<Surreal> Br_a = b.right * a; /// Br * a

        std::set<Surreal> negAl_Bl = NegateSet(a.left * b.left); /// - Al * Bl
        std::set<Surreal> negAr_Br = NegateSet(a.right * b.right); /// - Ar * Br

        std::set<Surreal> negAl_Br = NegateSet(a.left * b.right); /// - Al * Br
        std::set<Surreal> negAr_Bl = NegateSet(a.right * b.left); /// - Ar * Bl

        /// tempL1 = Al*b + a*Bl - Al*Bl
        std::set<Surreal> tempL1 = Al_b + Bl_a + negAl_Bl;
        //std::set<Surreal> tempL1 = ((a.left * b) + (b.left * a)) + NegateSet(a.left * b.left);

        /// tempL2 = Ar*b + a*Br - Ar*Br
        std::set<Surreal> tempL2 = Ar_b + Br_a + negAr_Br;
        //std::set<Surreal> tempL2 = ((a.right * b) + (b.right * a)) + NegateSet(a.right * b.right);

        /// tempR1 = Al*b + a*Br - Al*Br
        std::set<Surreal> tempR1 = Al_b + Br_a + negAl_Br;
        //std::set<Surreal> tempR1 = ((a.left * b) + (b.right * a)) + NegateSet(a.left * b.right);

        /// tempR2 = Ar*b + a*Bl - Ar*Bl
        std::set<Surreal> tempR2 = Ar_b + Bl_a + negAr_Bl;
        //std::set<Surreal> tempR2 = ((a.right * b) + (b.left * a)) + NegateSet(a.right * b.left);

        /// combine the sets to form both sides of the new Surreal
        tempL1.insert(tempL2.begin(), tempL2.end());
        tempR1.insert(tempR2.begin(), tempR2.end());

        Surreal res = Surreal(tempL1, tempR1);

        /// We check that the result does not have a simpler, equivalent representation in the addition lookup table.
        /// If it does, then use the simpler version instead. This prevents numbers with several terms
        /// in their sets from forming.
        ///
        /// "Simpler" means "less terms on the left and right side", for example { -1 | 1 } will be simplified to 0.

        /// store the number of terms in the result to reference it during simplification
        std::size_t resSize = res.left.size() + res.right.size();

        auto simplIter = Surreal::MultLookup.begin();
        while (simplIter != Surreal::MultLookup.end()) {
            if (res == simplIter->second) {

                /// found an equivalent value, check if it is "simpler"
                std::size_t eqSize = simplIter->second.left.size() + simplIter->second.right.size();
                if (resSize > eqSize) {
                    /// result is more complex than an equivalent value in the lookup table.
                    /// replace the result with the lookup value and exit the search
                    res = simplIter->second;
                    break;
                } else if (resSize < eqSize) {
                    /// result is less complex than an equivalent value in the table.
                    /// replace the lookup value with the result.
                    ///
                    /// This should not happen in normal use due to how the recursion happens,
                    /// however it is possible to trigger this by manually creating numbers.
                    simplIter->second = res;
                } else {
                    /// result is equivalent and has the same complexity, use lookup value instead
                    /// and exit the search.
                    ///
                    /// In practice, as soon as we reach a value that is equivalent and also
                    /// has the same complexity, we can assume that other equivalent values in the table
                    /// have the same representation.
                    res = simplIter->second;
                    break;
                }
            }
            simplIter++;
        }

        /// insert the result into the lookup table
        Surreal::MultLookup.emplace(std::pair<Surreal, Surreal>(std::minmax(a, b)), res);

        return res;
    }

    /// Arithmetic between sets of surreal numbers
    /// Used when one or both sides of the operator is a set of Surreals

    /// Set of Surreals + Surreal number
    ///
    /// \param sur_set: the set
    /// \param sur_num: the number
    /// \return the resulting set
    std::set<Surreal> operator+(std::set<Surreal> const &sur_set, const Surreal &sur_num) {
        std::set<Surreal> temp;
        for (Surreal const &elem : sur_set) { temp.insert(elem + sur_num); }
        return temp;
    }

    /// Set of Surreals + Set of Surreals
    ///
    /// \param sur_set1: the first set
    /// \param sur_set2: the second set
    /// \return the resulting set
    std::set<Surreal> operator+(std::set<Surreal> const &sur_set1, std::set<Surreal> const &sur_set2) {
        std::set<Surreal> temp;
        for (Surreal const &elem1 : sur_set1) {
            for (Surreal const &elem2 : sur_set2) {
                temp.insert(elem1 + elem2);
            }
        }
        return temp;
    }

    /// Negation of a Set of Surreals
    ///
    /// \param sur_set: the set to be negated
    /// \return the negated set
    std::set<Surreal> NegateSet(std::set<Surreal> const &sur_set) {
        std::set<Surreal> temp;
        for (Surreal const &elem : sur_set) { temp.insert(-elem); }
        return temp;
    }

    /// Set of Surreals * Surreal Number
    ///
    /// \param sur_set: the set
    /// \param sur_num: the number
    /// \return the resulting set
    std::set<Surreal> operator*(std::set<Surreal> const &sur_set, const Surreal &sur_num) {
        std::set<Surreal> temp;
        for (Surreal const &elem : sur_set) { temp.insert(elem * sur_num); }
        return temp;
    }

    /// Set of Surreals * Set of Surreals
    ///
    /// \param sur_set1: the first set
    /// \param sur_set2: the second set
    /// \return the resulting set
    std::set<Surreal> operator*(std::set<Surreal> const &sur_set1, std::set<Surreal> const &sur_set2) {
        std::set<Surreal> temp;
        for (Surreal const &elem1 : sur_set1) {
            for (Surreal const &elem2 : sur_set2) {
                temp.insert(elem1 * elem2);
            }
        }
        return temp;
    }

    /// Ordering between finite Surreals
    /// all order relations are derived from "less than or equal to"

    /// less than or equal to
    bool operator<=(Surreal const &a, Surreal const &b) {
        for (Surreal const &a_left_item : a.left) {
            if (b <= a_left_item) { return false; }
        }
        for (Surreal const &b_right_item : b.right) {
            if (b_right_item <= a) { return false; }
        }
        return true;
    }

    bool operator>=(Surreal const &a, Surreal const &b) { return (b <= a); }

    bool operator==(Surreal const &a, Surreal const &b) { return ((a <= b) && (b <= a)); }

    bool operator!=(Surreal const &a, Surreal const &b) { return !(a == b); }

    bool operator>(Surreal const &a, Surreal const &b) { return !(a <= b); }

    bool operator<(Surreal const &a, Surreal const &b) { return (b > a); }

    /// Conversion to Float
    ///
    /// Finite surreal numbers are equivalent to binary fractions.
    /// The conversion recursively computes the bounds of sets L and R
    /// as floats, and uses that to calculate the resulting float.
    ///
    /// \return the resulting float
    float Surreal::Float() const {

        /// recursively convert each number in sets L and R to float,
        /// put them into temporary sets of floats
        std::set<float> floatset_left;
        std::set<float> floatset_right;

        for (Surreal const &left_elem : left) { floatset_left.insert(left_elem.Float()); }
        for (Surreal const &right_elem : right) { floatset_right.insert(right_elem.Float()); }

        /// Using converted L and R sets, compute the result
        float result;
        if (floatset_left.empty()) {
            if (floatset_right.empty()) {
                /// both L and R are empty:  { | }
                /// The result is zero
                result = 0.0;
            } else {
                /// only L is empty: { | stuff ...}
                /// The result is offset from the smallest number in R by 1
                result = *floatset_right.begin() - (float) 1.0;
            }
        } else {
            if (floatset_right.empty()) {
                /// only R is empty: { ... stuff | }
                /// The result is offset from the greatest number in L by 1
                result = *floatset_left.rbegin() + (float) 1.0;
            } else {
                /// both L and R contain numbers: { .... stuff | stuff ...}
                /// The result lies exactly between the greatest number in L and the smallest number in R
                result = (*floatset_right.begin() + *floatset_left.rbegin()) / 2;
            }
        }
        return result;
    }

    /// Implicit float conversion
    /// calls the Float method
    ///
    /// \return the float
    Surreal::operator float() {
        return this->Float();
    }

    /// Verbose display
    /// Displays the number using only brackets and separators
    ///
    /// \return the string for the verbose form
    std::string Surreal::PrintVerbose() const {
        std::string tempstr;
        tempstr += "{ ";
        for (surreals::Surreal const &leftnum : left) {
            tempstr += leftnum.PrintVerbose();
            tempstr += " ";
        }
        tempstr += "| ";
        for (surreals::Surreal const &rightnum : right) {
            tempstr += rightnum.PrintVerbose();
            tempstr += " ";
        }
        tempstr += "}";
        return tempstr;
    }

    /// Hybrid display
    /// Prints the number using brackets and separators, shortening the children to floats after a set depth.
    ///
    /// \param depth : at which level the numbers are shortened to floats
    /// \return
    std::string Surreal::Print(int depth = 0) const {
        std::string tempstr;

        tempstr += "{ ";

        for (surreals::Surreal const &leftnum : left) {
            /// If we are at "depth 0", swap the terms for their float representations.
            /// Otherwise, recursively print the terms.
            if (depth > 0) { tempstr += leftnum.Print(depth - 1); }
            else { tempstr += std::to_string(leftnum.Float()); }
            tempstr += " ";
        }

        tempstr += "| ";

        for (surreals::Surreal const &rightnum : right) {
            /// If we are at "depth 0", swap the terms for their float representations.
            /// Otherwise, recursively print the terms.
            if (depth > 0) { tempstr += rightnum.Print(depth - 1); }
            else { tempstr += std::to_string(rightnum.Float()); }
            tempstr += " ";
        }

        tempstr += "}";
        return tempstr;
    }

    /// "Infinite" Surreals

    /// Constructor from two generating functions
    ///
    /// \param leftIn: the left generating function
    /// \param rightIn: the right generating function
    SurrealInf::SurrealInf(
            std::function<SurrealInf(int)> const &leftIn,
            std::function<SurrealInf(int)> const &rightIn,
            std::pair<int, int> const &sizes) {
        /// No checks are performed at construction. It is trusted that:
        /// 1) the left function generates numbers in non-strict ascending order
        /// 2) the right function generates numbers in non-strict descending order
        /// 3) any number generated by the right function is greater than any number generated by the left function
        this->left = leftIn;
        this->right = rightIn;
        this->leftSize = sizes.first;
        this->rightSize = sizes.second;
    }

    /// Constructs an "infinite" Surreal using a finite Surreal
    /// For each side, the generating function will output either a single number or nothing
    SurrealInf::SurrealInf(Surreal const &inputSur) {
        /// Take the greatest number on the left and the smallest number on the right (if the corresponding sets
        /// are not empty), recursively convert them into SurrealInf, then specify the generating functions
        /// to return those numbers.

        if (!inputSur.left.empty()) {
            /// left side of input Surreal isn't empty, specify the generating function
            SurrealInf leftNumber = SurrealInf(*(inputSur.left.rbegin()));
            std::function<SurrealInf(int)> tempLeft = [leftNumber](int) { return leftNumber; };

            this->left = tempLeft;
            this->leftSize = 1;

        } else {
            this->leftSize = 0;
        }

        if (!inputSur.right.empty()) {
            /// right side of input Surreal isn't empty, specify the generating function
            SurrealInf rightNumber = SurrealInf(*(inputSur.right.begin()));
            std::function<SurrealInf(int)> tempRight = [rightNumber](int) { return rightNumber; };

            this->right = tempRight;
            this->rightSize = 1;

        } else {
            this->rightSize = 0;
        }
    }

    /// SurrealInf float constructor chained to the Surreal float constructor
    SurrealInf::SurrealInf(float const &inputFloat) : SurrealInf::SurrealInf(Surreal(inputFloat)) {}

    /// SurrealInf int constructor chained to the Surreal int constructor
    SurrealInf::SurrealInf(int const &inputInt) : SurrealInf::SurrealInf(Surreal(inputInt)) {}

    /// Default constructor
    SurrealInf::SurrealInf() = default;

    /// Destructor
    SurrealInf::~SurrealInf() = default;

    /// "Get" functions for "infinite" Surreals

    /// Get Nth element of the left set.
    /// This method is not const, since the SurrealInf caches the result of the fetch.
    ///
    /// \param n: the requested index
    /// \return the Nth generated element
    SurrealInf SurrealInf::getLeft(int const &n) {

        /// Check the cache for the requested integer. If not found,
        /// generate the number, cache it and return it.
        auto cacheIter = leftCache.find(n);

        if (cacheIter != leftCache.end()) {
            /// number generated previously, return the cached value
            return cacheIter->second;
        } /// number not generated previously

        SurrealInf result = left(n);
        leftCache[n] = result;
        return result;
    }

    /// Get Nth element of the right set
    /// This method is not const, since the SurrealInf caches the result of the fetch.
    ///
    /// \param n: the requested index
    /// \return the Nth generated element
    SurrealInf SurrealInf::getRight(int const &n) {
        /// Check the cache for the requested integer. If not found,
        /// generate the number, cache it and return it.

        auto cacheIter = rightCache.find(n);

        if (cacheIter != rightCache.end()) {
            /// number generated previously, return the cached value
            return cacheIter->second;
        } /// number not generated previously

        SurrealInf result = right(n);
        rightCache[n] = result;
        return result;
    }

    /// Converts the SurrealInf into a Surreal, then converts that into a float. If the conversion to
    /// Surreal fails, returns NaN.
    float SurrealInf::Float() {
        try {
            Surreal temp = Surreal(*this);
            return temp.Float();
        } catch (const std::runtime_error &e) {
            return NAN;
        }
    }

    /// Hybrid display for SurrealInf
    /// Prints the number using brackets and separators, shortening the children to floats after a set depth.
    ///
    /// \param width: how many terms are computed in infinite sets
    /// \param depth: at which level the numbers are shortened to floats
    /// \return the string to display
    std::string SurrealInf::Print(int width = 5, int depth = 0) {
        std::string tempstr; /// temporary string we write to

        tempstr += "{ ";

        /// Print left side
        if (leftSize > 0) { /// left side has finite, nonzero size.
            for (int i = 0; i < leftSize; i++) {
                /// If we are at "depth 0", swap the terms for their float representations.
                /// Otherwise, recursively print the terms.
                if (depth > 0) { tempstr += getLeft(i).Print(width, depth - 1); }
                else { tempstr += std::to_string(getLeft(i).Float()); }
                tempstr += " ";
            }
        } else if (leftSize < 0 && width > 0) { /// left side has infinite size
            for (int i = 0; i < width; i++) {
                /// If we are at "depth 0", swap the terms for their float representations.
                /// Otherwise, recursively print the terms.
                if (depth > 0) { tempstr += getLeft(i).Print(width, depth - 1); }
                else { tempstr += std::to_string(getLeft(i).Float()); }
                tempstr += " ";
            }
            tempstr += "... ";
        }

        tempstr += "| ";

        /// Print right side
        if (rightSize > 0) { /// right side has finite, nonzero size.
            for (int i = rightSize - 1; i >= 0; i--) {
                /// If we are at "depth 0", swap the terms for their float representations.
                /// Otherwise, recursively print the terms.
                if (depth > 0) { tempstr += getRight(i).Print(width, depth - 1); }
                else { tempstr += std::to_string(getRight(i).Float()); }
                tempstr += " ";
            }
        } else if (rightSize < 0 && width > 0) { /// right side has infinite size
            tempstr += "... ";
            for (int i = width - 1; i >= 0; i--) {
                /// If we are at "depth 0", swap the terms for their float representations.
                /// Otherwise, recursively print the terms.
                if (depth > 0) { tempstr += getRight(i).Print(width, depth - 1); }
                else { tempstr += std::to_string(getRight(i).Float()); }
                tempstr += " ";
            }
        }

        tempstr += "}";
        return tempstr;
    }


    /// The "verbose" display function for "infinite" Surreals
    /// Displays the number using only brackets and separators
    ///
    /// \param width: how many terms are computed on each side
    /// \return the string to display
    std::string SurrealInf::PrintVerbose(int width = 5) {
        std::string tempstr;
        tempstr += "{ ";

        if (leftSize > 0) {
            for (int i = 0; i < leftSize; i++) {
                tempstr += getLeft(i).PrintVerbose();
                tempstr += " ";
            }
        } else if (leftSize < 0 && width > 0) {
            for (int i = 0; i < width; i++) {
                tempstr += getLeft(i).PrintVerbose();
                tempstr += " ";
            }
            tempstr += "... ";
        }

        tempstr += "| ";

        if (rightSize > 0) {
            for (int i = rightSize - 1; i >= 0; i--) {
                tempstr += getRight(i).PrintVerbose();
                tempstr += " ";
            }
        } else if (rightSize < 0 && width > 0) {
            tempstr += "... ";
            for (int i = width - 1; i >= 0; i--) {
                tempstr += getRight(i).PrintVerbose();
                tempstr += " ";
            }
        }

        tempstr += "}";
        return tempstr;
    }

} //surreals

/// ostream display uses the Print method
std::ostream &operator<<(std::ostream &os, surreals::Surreal const &number) {
    os << number.Print(0);
    return os;
}

std::ostream &operator<<(std::ostream &os, surreals::SurrealInf &number) {
    os << number.Print(5, 0);
    return os;
}