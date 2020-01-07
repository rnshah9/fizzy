#include "parser.hpp"

namespace fizzy
{
namespace
{
template <typename T>
inline void push(bytes& b, T value)
{
    b.resize(b.size() + sizeof(value));
    __builtin_memcpy(&b[b.size() - sizeof(value)], &value, sizeof(value));
}
}  // namespace

parser_result<code> parse_expr(const uint8_t* pos)
{
    code c;
    instr i;
    do
    {
        i = static_cast<instr>(*pos++);
        switch (i)
        {
        default:
            throw parser_error{"invalid instruction " + std::to_string(*pos)};

        case instr::unreachable:
        case instr::nop:
        case instr::end:
        case instr::i32_add:
            break;

        case instr::local_get:
        case instr::local_set:
        case instr::local_tee: {
            uint32_t imm;
            std::tie(imm, pos) = leb128u_decode<uint32_t>(pos);
            push(c.immediates, imm);
            break;
        }
        }
        c.instructions.emplace_back(i);
    } while (i != instr::end);
    return {c, pos};
}
}  // namespace fizzy
